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

static alloc_nd_t *first_nd = NULL;

inline void *alloc_pages(size_t num) {
  uintptr_t ptr;
  asm volatile("int $0x30" : "=c" (ptr) : "a" (SYSCALL_MALLOC), "b" (num));
  return (void*) ptr;
}

inline void free_pages(uintptr_t ptr, size_t num) {
  asm volatile("int $0x30" : : "a" (SYSCALL_MFREE), "b" (ptr), "c" (num));
}

void init_heap(void) {
  first_nd = (alloc_nd_t*) alloc_pages(1);
  first_nd->bytes = PAGE_SIZE;
  first_nd->next_nd = NULL;
  first_nd->prev_nd = NULL;
}

void insert_node(alloc_nd_t *node) {
  first_nd->prev_nd = node;
  node->next_nd = first_nd;
  node->prev_nd = NULL;
  first_nd = node;
}

void remove_node(alloc_nd_t *node) {
  if(node->prev_nd) node->prev_nd->next_nd = node->next_nd;
  if(node->next_nd) node->next_nd->prev_nd = node->prev_nd;
}

void *malloc(size_t bytes) {
  if( bytes <= PAGE_SIZE && bytes > (PAGE_SIZE - sizeof(alloc_nd_t)) ) {
    return alloc_pages(1);
  }
  
  alloc_nd_t *node = first_nd;
  
  // search for free nodes
  int node_found = 0;
  while(node != NULL && !node_found) {
    if(node->bytes >= bytes) { // is enough space?
      node_found = 1;
    } else {
      node = node->next_nd;
    }
  }
  
  // nothing avaiable - create new node
  if(! node_found) {
    size_t pages = bytes / PAGE_SIZE + 1;
    uintptr_t vaddr = (uintptr_t) alloc_pages(pages);
    node = (alloc_nd_t*) vaddr;
    
    node->bytes = PAGE_SIZE * pages;
  }
  
  // is there some space for more?
  size_t rest = node->bytes - bytes;
  if(rest > sizeof(alloc_nd_t)) {
    alloc_nd_t *new_node = (alloc_nd_t*) node + sizeof(alloc_nd_t);
    new_node->bytes = rest - sizeof(alloc_nd_t);
    node->bytes = bytes;
    insert_node(new_node);
  }
  
  remove_node(node);
  return (void*) node + sizeof(alloc_nd_t);
}

void *calloc(size_t num, size_t size) {
  void *ptr = malloc(num * size);
  memset(ptr, 0, num * size);
  return ptr;
}

void *realloc(void *ptr, size_t bytes) {
  void *nptr = malloc(bytes);
  alloc_nd_t *nd = ptr - sizeof(alloc_nd_t);
  memmove(nptr, ptr, nd->bytes);
  free(ptr);
  
  return nptr;
}

void free(void *ptr) {
  if((uintptr_t)ptr & 0xFFF) {
    alloc_nd_t *node = (alloc_nd_t*) ptr - sizeof(alloc_nd_t);
    insert_node(node);
  } else {
    free_pages((uintptr_t) ptr, 1);
  }
}
