/**
 *  kernel/mm/heap.c
 *
 *  (C) Copyright 2012 Michael Sippel
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

static alloc_nd_t *first_nd;

void init_heap(void) {
  first_nd = mk_nd(vmm_alloc(), PAGE_SIZE, NULL, NULL);
}

alloc_nd_t *mk_nd(uintptr_t addr, size_t bytes, alloc_nd_t *next, alloc_nd_t *prev) {
  alloc_nd_t *node = (alloc_nd_t*) addr;
  node->bytes = bytes;
  node->next_nd = next;
  node->prev_nd = prev;
  
  return node;
}

void *malloc(size_t bytes) {
  bytes += sizeof(alloc_nd_t);
  alloc_nd_t *node = first_nd;
  while (node != NULL) {
    if(node->bytes >= bytes) { // is enough space?
      if(node->bytes > bytes) { // is there some space for more?
	size_t rest = node->bytes - bytes;
	first_nd->prev_nd = mk_nd((uintptr_t)node+node->bytes-rest, rest, 
				  first_nd, NULL);
	first_nd = first_nd->prev_nd;
      }
      node->prev_nd->next_nd = node->next_nd;
      node->next_nd->prev_nd = node->prev_nd;
      return (void*) node + sizeof(alloc_nd_t);
    } else {
      node = node->next_nd;
    }
  }
  first_nd->prev_nd = mk_nd(vmm_alloc_area(bytes/PAGE_SIZE+1), 
			    PAGE_SIZE*((size_t)bytes/PAGE_SIZE), 
			    first_nd, NULL);
  first_nd = first_nd->prev_nd;
  
  return NULL;
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
  alloc_nd_t *node = (alloc_nd_t*) ptr - sizeof(alloc_nd_t);
  first_nd->prev_nd = node;
  node->next_nd = first_nd;
  first_nd = node;
}
