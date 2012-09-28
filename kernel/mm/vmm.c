/**
 *  kernel/mm/vmm.c
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
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <debug/panic.h>
#include <driver/console.h>
#include <multiboot.h>
#include <mm.h>

static vmm_context_t *current_context;
static vmm_context_t *kernel_context;
static bool paging_enabled = FALSE;
static uint32_t cr0;

void init_vmm(void) {
  kernel_context = vmm_create_context();
  
  // create kernel-mapping
  uintptr_t vaddr = 0;
  uintptr_t paddr = 0;
  
  // before kernel
  while(paddr < (uintptr_t) &kernel_end) {
    vmm_map_page(kernel_context, paddr, paddr);
    paddr += PAGE_SIZE;
  }
  // kernel
  vaddr = 0xC0000000;
  paddr = (uintptr_t) &kernel_start;
  while(paddr < (uintptr_t) &kernel_end) {
    vmm_map_page(kernel_context, vaddr, paddr);
    vaddr += PAGE_SIZE;
    paddr += PAGE_SIZE;
  }
  // videomemory (0xB8000 - 0xBFFFF)
  paddr = 0xB8000;
  while(paddr < 0xBFFFF) {
    vmm_map_page(kernel_context, paddr, paddr);
    paddr += PAGE_SIZE;
  }
  
  vmm_activate_context(kernel_context);
  vmm_enable();
}

void vmm_enable(void) {
  asm volatile("mov %%cr0, %0" : "=r" (cr0));
  cr0 |= (1 << 31);
  asm volatile("mov %0, %%cr0" : : "r" (cr0));
  paging_enabled = TRUE;
}

void vmm_disable(void) {
  asm volatile("mov %%cr0, %0" : "=r" (cr0));
  cr0 &= ~(1 << 31);
  asm volatile("mov %0, %%cr0" : : "r" (cr0));
  paging_enabled = FALSE;
}

void vmm_create_pagedir(vmm_context_t *context) {
  vmm_pd_t *pagedir = pmm_alloc();
  int i;
  for(i = 0; i < PD_SIZE; i++) {
    pagedir[i] = 0;
  }
  
  uintptr_t vaddr = context->alloc_offset;
  context->alloc_offset += (PD_SIZE / PAGE_SIZE) * PAGE_SIZE;
  pagedir[PT_INDEX(PAGE_INDEX(vaddr))] = (vmm_pd_t) pagedir | context->flags;
  
  if(paging_enabled)
    context->pagedir = vaddr;
  else
    context->pagedir = pagedir;
  context->pagedir_paddr = (uintptr_t) pagedir;
}

vmm_pt_t *vmm_create_pagetable(vmm_context_t *context, int index) {
  vmm_pt_t *pagetable = pmm_alloc();
  int i;
  for(i = 0; i < PT_SIZE; i++) {
    pagetable[i] = 0;
  }
  
  vmm_pd_t *pagedir = context->pagedir_paddr;
  context->pagedir[index] = (vmm_pt_t) pagetable | context->flags;
  return pagetable;
}

vmm_context_t *vmm_create_context(void) {
  vmm_context_t *context = pmm_alloc();
  context->flags = VMM_PRESENT | VMM_WRITE;
  context->alloc_offset = 1*PAGE_SIZE;
  vmm_create_pagedir(context);
  
  return context;
}

inline void vmm_activate_context(vmm_context_t *context) {
  current_context = context;
  asm volatile("mov %0, %%cr3" : : "r" (context->pagedir_paddr));
}

inline void vmm_flush_tlb(uintptr_t vaddr) {
  asm volatile("invlpg %0" : : "m" (*(char*) vaddr));
}

int vmm_map_page(vmm_context_t *context, uintptr_t vaddr, uintptr_t paddr) {
  uint32_t page_index = PAGE_INDEX(vaddr);
  uint32_t pd_index = PD_INDEX(page_index);
  uint32_t pt_index = PT_INDEX(page_index);
  uint32_t *pagetable;
  
  if( (vaddr & 0xfff) || (paddr & 0xfff) ) {
    char msg[70];
    sprintf(msg, "vmm_map_page(): Can't map 0x%x to 0x%x! No 4k alignment!", vaddr, paddr);
    panic(msg);
    return -1;
  } else {
    if(context->pagedir[pd_index] & VMM_PRESENT) {
      pagetable = (vmm_pt_t*) (context->pagedir[pd_index] & ~0xfff);
    } else {
      pagetable = vmm_create_pagetable(context, pd_index);
    }
    
    pagetable[pt_index] = paddr | context->flags;
    vmm_flush_tlb(vaddr);
    
    return 0;
  }
}

void *vmm_alloc(void) {
  uintptr_t paddr = (uintptr_t) pmm_alloc();
  uintptr_t vaddr = current_context->alloc_offset;
  vmm_map_page(current_context, vaddr, paddr);
  current_context->alloc_offset += PAGE_SIZE;
  
  return (void*) vaddr;
}
