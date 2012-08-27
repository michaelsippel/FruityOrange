/**
 * kernel/mm/vmm.c
 *
 * (C) Copyright 2012 Michael Sippel
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdint.h>
#include <stddef.h>

#include <console.h>
#include <panic.h>
#include <multiboot.h>
#include <mm.h>

static vmm_context_t *kernel_context;
static vmm_context_t *current_context;

void init_vmm(void) {
  uint32_t cr0;
  
  kernel_context->flags = VMM_KERNEL_FLAGS;
  kernel_context = vmm_create_context();
  vmm_activate_context(kernel_context);
  
  asm volatile("mov %%cr0, %0" : "=r" (cr0));
  cr0 |= (1 << 31);
  asm volatile("mov %0, %%cr0" : : "r" (cr0));
}

void vmm_map_kernel(vmm_context_t *context) {
  uintptr_t vaddr = 0;
  uintptr_t paddr = 0;
  
  for (paddr = 0; paddr < 4096 * 1024; paddr += PAGE_SIZE) {
    vmm_map_page(context, vaddr, paddr);
    vaddr += PAGE_SIZE;
  }
}

int vmm_map_page(vmm_context_t *context, uintptr_t vaddr, uintptr_t paddr) {
  int page = vaddr / PAGE_SIZE;
  int pd_index = page / PAGE_TABLE_SIZE;
  int pt_index = page % PAGE_TABLE_SIZE;
  
  uint32_t *page_table;
  
  // 4k-alignment needed
  if ((vaddr & 0xFFF) || (paddr & 0xFFF)) {
    setColor(0xf4);
    printf("\nvmm_map_page(): Can't map 0x%x to 0x%x!\n", vaddr, paddr);
    panic("vmm-mapping error. No 4k alignment!");
    return -1;
  }
  
  // Check, if pagetable is already available
  if (context->pagedir[pd_index] & PTE_PRESENT) {
      // use available pagetable
      page_table = (uint32_t*) (context->pagedir[pd_index] & ~0xFFF);
  } else {
      // create new pagetable
      page_table = vmm_create_pagetable(context, pd_index);
  }
  
  // entry the mapping in pagetable
  page_table[pt_index] = paddr | context->flags;
  asm volatile("invlpg %0" : : "m" (*(char*)vaddr));
  
  return 0;
}

void* vmm_alloc(void) {
  uintptr_t paddr = (uintptr_t) pmm_alloc();
  uintptr_t vaddr = current_context->alloc_offset;
  current_context->alloc_offset += PAGE_SIZE;
  vmm_map_page(current_context, vaddr, paddr);
  
  return (void*) vaddr;
}

uint32_t* vmm_create_pagetable(vmm_context_t *context, int index) {
  uint32_t *page_table = pmm_alloc();
  int i;
  for (i = 0; i < PAGE_TABLE_SIZE; i++) {
    page_table[i] = 0;
  }
  context->pagedir[index] = (uint32_t) page_table | context->flags;
  
  return page_table;
}

void vmm_create_pagedir(vmm_context_t *context) {
  int i;
  context->pagedir_paddr = (uint32_t) pmm_alloc();
  context->pagedir = (uint32_t*) 0x1000;
  vmm_map_page(context, (uintptr_t) context->pagedir, context->pagedir_paddr);
  context->alloc_offset = 0x2000;
  
  for(i = 0; i < PAGE_DIR_SIZE; i++) {
    context->pagedir[i] = 0;
  }
}

vmm_context_t* vmm_create_context(void) {
  vmm_context_t *context = pmm_alloc();
  context->alloc_offset = 0;
  
  vmm_create_pagedir(context);// create pagedirectory
  vmm_map_kernel(context);// entry kernel-mapping
  
  return context;
}

void vmm_activate_context(vmm_context_t *context) {
  current_context = context;
  asm volatile("mov %0, %%cr3" : : "r" (context->pagedir_paddr));
}
