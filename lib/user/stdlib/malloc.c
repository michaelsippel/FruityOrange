/**
 *  lib/user/stdlib/malloc.c
 *
 *  (C) Copyright 2013 Michael Sippel
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
#include <sys/syscalls.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

static alloc_block_t *first_block = NULL;

inline void *alloc_pages(size_t num) {
  uintptr_t ptr;
  asm volatile("int $0x30" : "=c" (ptr) : "a" (SYSCALL_MALLOC), "b" (num));
  return (void*) ptr;
}

inline void free_pages(uintptr_t ptr, size_t num) {
  asm volatile("int $0x30" : : "a" (SYSCALL_MFREE), "b" (ptr), "c" (num));
}

void init_heap(void) {
  uintptr_t addr = alloc_pages(1);
  first_block->base = addr + sizeof(alloc_block_t);
  first_block->size = PAGE_SIZE - sizeof(alloc_block_t);
  first_block->next = NULL;
  first_block->prev = NULL;
}

alloc_block_t *heap_increase(size_t pages) {
  alloc_block_t *node = first_block;
  alloc_block_t *new  = alloc_pages(pages);
  
  new->size = pages*PAGE_SIZE - sizeof(alloc_block_t);
  new->base = new + sizeof(alloc_block_t);
  
  new->prev = NULL;
  new->next = first_block;
  first_block->prev = new;
  first_block = new;
  
  return new;
}

void *malloc(size_t bytes) {
  if( bytes <= PAGE_SIZE && bytes > (PAGE_SIZE - sizeof(alloc_block_t)) ) {
    return alloc_pages(1);
  }
  
  alloc_block_t *node = first_block;

  if(node == NULL) {
    init_heap();
  }  

  // search for free nodes
  while(node != NULL) {
    if(node->size >= bytes) { // is enough space?
      if(node->prev == NULL) {
        heap_increase(1);
      }
      node->prev->next = node->next;
      if(node->next != NULL) {
        node->next->prev = node->prev;
      }
      
      node->prev = NULL;
      node->next = NULL;
      
      return node->base;
    } else {
      node = node->next;
    }
  }
  
  node = heap_increase((bytes+sizeof(alloc_block_t)+PAGE_SIZE)/PAGE_SIZE);
  if(node != NULL) {
    return node->base;
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
    node->prev = NULL;
    node->next = first_block;
    first_block->prev = node;
    first_block = node;
  } else {
    free_pages(ptr, 1);
  }
}

