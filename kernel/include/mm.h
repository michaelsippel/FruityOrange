/**
 *  kernel/include/mm.h
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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _MM_H
#define _MM_H

#include <stdint.h>
#include <stddef.h>
#include <multiboot.h>

#define PMM_FREE 1
#define PMM_USED 0
#define PAGE_SIZE 0x1000
#define PAGE_TABLE_SIZE 1024
#define PAGE_DIR_SIZE 1024


#define PTE_PRESENT 0x01
#define PTE_WRITE   0x02
#define PTE_USER    0x04

#define VMM_KERNEL_FLAGS PTE_PRESENT | PTE_WRITE
#define VMM_USER_FLAGS   PTE_PRESENT | PTE_WRITE | PTE_USER

typedef struct vmm_context {
  uint32_t *pagedir;
  uintptr_t pagedir_paddr;
  uintptr_t alloc_offset;
  uint8_t flags;
} vmm_context_t;

typedef struct alloc_nd {
  int bytes;
  struct alloc_nd *prev_nd;
  struct alloc_nd *next_nd;
} alloc_nd_t;

// pmm
void init_pmm(struct multiboot_info *mb_info);
void *pmm_alloc(void);
void pmm_free(void *ptr);
void pmm_mark_used(void *ptr);

// vmm
void init_vmm(void);
void vmm_map_kernel(vmm_context_t *context);
int vmm_map_page(vmm_context_t *context, uintptr_t vaddr, uintptr_t paddr);
void *vmm_alloc(void);
void *vmm_alloc_pages(size_t num);
uint32_t *vmm_create_pagetable(vmm_context_t *context, int index);
void vmm_create_pagedir(vmm_context_t *context);
vmm_context_t *vmm_create_context(void);
void vmm_activate_context(vmm_context_t *context);

// heap
void init_heap(void);
alloc_nd_t *mk_nd(uintptr_t addr, size_t bytes, alloc_nd_t *next, alloc_nd_t *prev);
void *malloc(size_t bytes);
void *calloc(size_t num, size_t size);
void *realloc(void *ptr, size_t bytes);
void free(void *ptr);

#endif
