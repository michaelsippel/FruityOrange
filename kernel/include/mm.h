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
#define PD_SIZE 1024
#define PT_SIZE 1024

#define PAGE_INDEX(x) (x / PAGE_SIZE)
#define PD_INDEX(x) (x / PT_SIZE)
#define PT_INDEX(x) (x % PT_SIZE)

#define VMM_PRESENT 0x01
#define VMM_WRITE   0x02
#define VMM_USER    0x04
#define VMM_KERNEL_FLAGS VMM_PRESENT | VMM_WRITE
#define VMM_USER_FLAGS   VMM_PRESENT | VMM_WRITE | VMM_USER

typedef uint32_t* vmm_pd_t;
typedef uint32_t* vmm_pt_t;

typedef struct vmm_context {
  vmm_pd_t pagedir;
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
void vmm_enable(void);
void vmm_disable(void);
vmm_pd_t vmm_create_pagedir(vmm_context_t *context);
vmm_pt_t vmm_create_pagetable(vmm_context_t *context, int index);
vmm_pt_t vmm_get_pagetable(vmm_context_t *context, int index);
vmm_context_t *vmm_create_context(uint8_t flags);
int vmm_map_page(vmm_context_t *context, uintptr_t vaddr, uintptr_t paddr);
void *vmm_alloc(void);
void *vmm_alloc_area(size_t num);
inline void vmm_activate_context(vmm_context_t *context);
inline void vmm_flush_tld(uintptr_t vaddr);
#ifndef _VMM_C
extern vmm_context_t* current_context;
#endif

// heap
void init_heap(void);
alloc_nd_t *mk_nd(uintptr_t addr, size_t bytes, alloc_nd_t *next, alloc_nd_t *prev);
void *malloc(size_t bytes);
void *calloc(size_t num, size_t size);
void *realloc(void *ptr, size_t bytes);
void free(void *ptr);

#endif
