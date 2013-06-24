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

void *alloc_pages(size_t num) {
  uintptr_t ptr;
  asm volatile("int $0x30" : "=c" (ptr) : "a" (SYSCALL_MALLOC), "b" (num));
  return (void*) ptr;
}

void free_pages(uintptr_t ptr, size_t num) {
  asm volatile("int $0x30" : : "a" (SYSCALL_MFREE), "b" (ptr), "c" (num));
}

alloc_block_t *heap_insert_block(alloc_block_t *block) {
  if(first_block != NULL) {
    first_block->prev = block;
  }
  
  block->next = first_block;
  block->prev = NULL;
  first_block = block;
  
  return block;
}

alloc_block_t *heap_increase(size_t bytes) {
  size_t pages = ( bytes + sizeof(alloc_block_t) + PAGE_SIZE ) / PAGE_SIZE;
  
  uintptr_t vaddr = (uintptr_t) alloc_pages(pages);
  alloc_block_t *block = vaddr;
  block->size = bytes;
  block->base = vaddr + sizeof(alloc_block_t);
  block->status = HEAP_STATUS_FREE;
  
  block->next = NULL;
  block->prev = NULL;
  
  //heap_insert_block(block);
  
  return block;
}

void *malloc(size_t bytes) {
  size_t pages = ( bytes + PAGE_SIZE ) / PAGE_SIZE;
  
  return alloc_pages(pages);
  
  alloc_block_t *block = first_block;
  while(block != NULL) {
    if(block->size >= bytes && block->status == HEAP_STATUS_FREE) {
      block->status = HEAP_STATUS_USED;
      return block->base;
    } else {
      block = block->next;
    }
  }
  
  block = heap_increase(bytes);
  if(block != NULL) {
    block->status = HEAP_STATUS_USED;
    return block->base;
  }
  
  return NULL;
}

void *calloc(size_t num, size_t size) {
  void *ptr = malloc(num * size);
  memset(ptr, 0, num * size);
  return ptr;
}

void *realloc(void *ptr, size_t bytes) {
  void *nptr;
  //alloc_block_t *block = ptr - sizeof(alloc_block_t);
  
  //if(bytes > block->size) {
    nptr = malloc(bytes);
    
    if(nptr != NULL) {
      memmove(nptr, ptr, bytes);
      free(ptr);
    }
  //}
  
  return nptr;
}

void free(void *ptr) {
  //alloc_block_t *block = ptr - sizeof(alloc_block_t);
  //block->status = HEAP_STATUS_FREE;
}

