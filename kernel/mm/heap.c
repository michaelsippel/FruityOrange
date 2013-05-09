/**
 *  kernel/mm/heap.c
 *
 *  (C) Copyright 2012-2013 Michael Sippel
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <mm.h>

static alloc_block_t *first_block = NULL;
static alloc_block_t *first_user_block = NULL;
static int modus = HEAP_MODUS_KERNEL;

void init_heap(void) {
  uintptr_t addr = vmm_automap_kernel_page(current_context, (uintptr_t) pmm_alloc());
  first_block = addr;
  first_block->base = addr + sizeof(alloc_block_t);
  first_block->size = PAGE_SIZE - sizeof(alloc_block_t);
  
  first_block->next = NULL;
  first_block->prev = NULL;
  
  addr = vmm_automap_user_page(current_context, (uintptr_t) pmm_alloc());
  first_user_block = addr;
  first_user_block->base = addr + sizeof(alloc_block_t);
  first_user_block->size = PAGE_SIZE - sizeof(alloc_block_t);
  
  first_user_block->next = NULL;
  first_user_block->prev = NULL;
}

void heap_change_mode(int new_modus) {
  modus = new_modus;
}

void insert_block(alloc_block_t *block) {
  if(modus == HEAP_MODUS_USER) {
    block->next = first_user_block;
    block->prev = NULL;
    first_user_block->prev = block;
    first_user_block = block;
  } else {
    block->next = first_block;
    block->prev = NULL;
    first_block->prev = block;
    first_block = block;
  }
}

alloc_block_t *heap_increase(size_t pages) {
  uintptr_t paddr = NULL;
  uintptr_t vaddr = NULL;
  
  if(modus == HEAP_MODUS_USER)
    vaddr = vmm_find(current_context, pages, VADDR_USER_START, VADDR_USER_END);
  else
    vaddr = vmm_find(current_context, pages, VADDR_KERNEL_HEAP_START, VADDR_KERNEL_HEAP_END);
  
  int i;
  for(i = 0; i < pages; i++) {
    paddr = pmm_alloc();
    vmm_map_page(current_context, vaddr + i*PAGE_SIZE, paddr, VMM_KERNEL_FLAGS);
  }
  
  alloc_block_t *new  = vaddr;
  
  new->size = pages*PAGE_SIZE - sizeof(alloc_block_t);
  new->base = vaddr + sizeof(alloc_block_t);
  
  // insert into list
  insert_block(new);
  
  return new;
}

void *malloc(size_t bytes) {
  if( bytes <= PAGE_SIZE && bytes > (PAGE_SIZE - sizeof(alloc_block_t)) ) {
    if(modus == HEAP_MODUS_USER)
      return vmm_automap_user_page(current_context, (uintptr_t) pmm_alloc());
    else
      return vmm_automap_kernel_page(current_context, (uintptr_t) pmm_alloc());
  }
  
  alloc_block_t *node = first_block->next;
  // search for free nodes
  while(node != NULL) {
    if(node->size >= bytes) { // is enough space?
      // remove from list
      node->prev->next = node->next;
      node->next->prev = node->prev;
      
      return node->base;
    } else {
      node = node->next;
    }
  }
  
  node = heap_increase((bytes+sizeof(alloc_block_t)+PAGE_SIZE)/PAGE_SIZE);
  if(node != NULL) {
    return malloc(bytes);
  }
  
  return NULL;
}

void *calloc(size_t num, size_t size) {
  void *ptr = malloc(num * size);
  memset(ptr, 0, num * size);
  return ptr;
}

void *realloc(void *ptr, size_t bytes) {
  void *nptr = malloc(bytes);
  alloc_block_t *nd = ptr - sizeof(alloc_block_t);
  memmove(nptr, ptr, nd->size);
  free(ptr);
  
  return nptr;
}

void free(void *ptr) {
  if((uintptr_t)ptr & (~PAGE_MASK)) {
    alloc_block_t *node = (alloc_block_t*) ptr - sizeof(alloc_block_t);
    
    // insert into list
    insert_block(node);
  } else {
    vmm_unmap_page(current_context, (uintptr_t) ptr);
  }
}

