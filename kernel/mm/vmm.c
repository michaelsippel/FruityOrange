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
#define _VMM_C

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#include <debug/debug.h>
#include <debug/panic.h>
#include <driver/console.h>
#include <multiboot.h>
#include <mm.h>

vmm_context_t *current_context;
vmm_context_t *kernel_context;
static bool paging_enabled = FALSE;
static uint32_t cr0;

void init_vmm(void) {
  kernel_context = pmm_alloc();
  memclr(kernel_context, PAGE_SIZE);
  
  kernel_context->flags = VMM_KERNEL_FLAGS;
  kernel_context->alloc_offset = 1;
  
  vmm_pd_t pagedir = pmm_alloc();
  memclr(pagedir, PAGE_SIZE);
//   pagedir[PD_INDEX(PAGE_INDEX(VADDR_PT_START))] = (uint32_t) pagedir | VMM_KERNEL_FLAGS | VMM_PRESENT;
  kernel_context->pagedir = pagedir;
  kernel_context->pagedir_paddr = (uintptr_t) pagedir;
  
  //create kernel pagetables
//   int i;
//   FOR_KERNEL_PTS(i) {
//     vmm_create_pagetable(kernel_context, i);
//   }
  
  // create kernel-mapping
  uintptr_t vaddr = 0;
  uintptr_t paddr = 0;
  
  // until kernel_end 1:1 mapping
  while(paddr < (uintptr_t) &kernel_end) {
    vmm_map_page(kernel_context, paddr, paddr);
    paddr += PAGE_SIZE;
  }
  // kernel
  vaddr = VADDR_KERNEL_START;
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
  
  vmm_map_page(kernel_context, VADDR_PD, (uintptr_t) kernel_context->pagedir);
  vmm_map_page(kernel_context, VADDR_CONTEXT, (uintptr_t) kernel_context);
  kernel_context->pagedir = (vmm_pd_t) VADDR_PD;
  kernel_context = (vmm_context_t*) VADDR_CONTEXT;
  
  current_context = kernel_context;
  vmm_enable();
}

inline void vmm_enable(void) {
  asm volatile("mov %%cr0, %0" : "=r" (cr0));
  cr0 |= (1 << 31);
  asm volatile("mov %0, %%cr0" : : "r" (cr0));
  paging_enabled = TRUE;
}

inline void vmm_disable(void) {
  asm volatile("mov %%cr0, %0" : "=r" (cr0));
  cr0 &= ~(1 << 31);
  asm volatile("mov %0, %%cr0" : : "r" (cr0));
  paging_enabled = FALSE;
}

void vmm_entry_kernelmapping(vmm_context_t *context) {
  int i;
  FOR_KERNEL_PTS(i) {
    context->pagedir[i] = kernel_context->pagedir[i];
  }
}

vmm_pt_t vmm_create_pagetable(vmm_context_t *context, int index) {
  vmm_pt_t pagetable = pmm_alloc();
  context->pagedir[index] = (uint32_t) pagetable | context->flags | VMM_PRESENT;
  vmm_map_page(context, PT_VADDR(index), pagetable);
  
  pagetable = vmm_get_pagetable(context, index);
  memclr(pagetable, PAGE_SIZE);
  
  return pagetable;
}

vmm_pt_t vmm_get_pagetable(vmm_context_t *context, int index) {
  vmm_pt_t pagetable;
  
  if(! context->pagedir[index] & VMM_PRESENT) {
    return NULL;
  }
  
  if(paging_enabled) {
    if(context != current_context) {
      pagetable = (vmm_pt_t) PT_PADDR(context, index);
      uintptr_t vaddr = (uintptr_t) vmm_find_free_page(current_context);
      vmm_map_page(current_context, vaddr, (uintptr_t) pagetable);
      pagetable = (vmm_pt_t) vaddr;
    } else {
      pagetable = (vmm_pt_t) PT_VADDR(index);
    }
  } else {
    pagetable = (vmm_pt_t) PT_PADDR(context, index);
  }
  
  return pagetable;
}

vmm_context_t *vmm_create_context(uint8_t flags) {
  vmm_context_t *context;
  uintptr_t paddr = (uintptr_t) pmm_alloc();
  uintptr_t vaddr = (uintptr_t) vmm_find_free_page(current_context);
  
  vmm_map_page(current_context, vaddr, (uintptr_t) paddr);
  context = (vmm_context_t*) vaddr;
  
  memclr(context, PAGE_SIZE);
  context->flags = flags;
  context->alloc_offset = 1;
  
  // create pagedirectory
  vmm_pd_t pagedir;
  uintptr_t pd_paddr = (uintptr_t) pmm_alloc();
  uintptr_t pd_vaddr = (uintptr_t) vmm_find_free_page(current_context);
  vmm_map_page(current_context, pd_vaddr, pd_paddr);
  pagedir = (vmm_pd_t) pd_vaddr;
  
  memclr(pagedir, PAGE_SIZE);
  vmm_map_page(context, VADDR_PD, pd_paddr);
  pagedir[PD_INDEX(PAGE_INDEX(VADDR_PT_START))] = (uint32_t) pd_paddr | context->flags | VMM_PRESENT;
  
  context->pagedir = pagedir;
  context->pagedir_paddr = pd_paddr;
  
  vmm_map_page(context, VADDR_CONTEXT, paddr);
  
  if(kernel_context != NULL) {
    vmm_entry_kernelmapping(context);
  }
  
  return context;
}

inline void vmm_activate_context(vmm_context_t *context) {
  if(current_context != context || current_context == NULL) {
    current_context = context;
    asm volatile("mov %0, %%cr3" : : "r" (context->pagedir_paddr));
  }
}

inline void vmm_flush_tlb(uintptr_t vaddr) {
  asm volatile("invlpg %0" : : "m" (*(char*) vaddr));
}

int vmm_map_page(vmm_context_t *context, uintptr_t vaddr, uintptr_t paddr) {
  uint32_t page_index = PAGE_INDEX(vaddr);
  uint32_t pd_index = PD_INDEX(page_index);
  uint32_t pt_index = PT_INDEX(page_index);
  vmm_pt_t pagetable;
  
  if( (vaddr & ~PAGE_MASK) || (paddr & ~PAGE_MASK) ) {
    char msg[70];
    sprintf(msg, "vmm_map_page(0x%x): Can't map 0x%x to 0x%x! No 4k alignment!", context, vaddr, paddr);
    panic(msg);
    return -1;
  } else {
    if(context->pagedir[pd_index] & VMM_PRESENT) {
      pagetable = vmm_get_pagetable(context, pd_index);
    } else {
      pagetable = vmm_create_pagetable(context, pd_index);
    }
    
    pagetable[pt_index] = (uint32_t) paddr | context->flags | VMM_PRESENT;
    vmm_flush_tlb(vaddr);
  }
  
  return 0;
}

void *vmm_find_free_page(vmm_context_t *context) {
  uintptr_t vaddr;
  vaddr = (uintptr_t) context->alloc_offset * PAGE_SIZE;
  context->alloc_offset++;

  return (void*) vaddr;
}

void *vmm_alloc(void) {
  uintptr_t paddr = (uintptr_t) pmm_alloc();
  uintptr_t vaddr = (uintptr_t) vmm_find_free_page(current_context);
  vmm_map_page(current_context, vaddr, paddr);
  
  return (void*) vaddr;
}
