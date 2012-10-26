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

#define VMM_DEBUG 1

vmm_context_t *current_context;
vmm_context_t *kernel_context;
static bool paging_enabled = FALSE;
static uint32_t cr0;

void init_vmm(void) {
  kernel_context = vmm_create_context(VMM_KERNEL_FLAGS);
  
  //create kernel pagetables
  int i;
  FOR_KERNEL_PTS(i) {
    vmm_create_pagetable(kernel_context, i);
  }
  
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

void vmm_entry_kernelmapping(vmm_context_t *context) {
  int i;
  FOR_KERNEL_PTS(i) {
    context->pagedir[i] = kernel_context->pagedir[i];
  }
}

vmm_pt_t vmm_create_pagetable(vmm_context_t *context, int index) {
  vmm_pt_t pagetable = pmm_alloc();
  memclr(pagetable, PAGE_SIZE);
  
  debug(VMM_DEBUG, "vmm_create_pagetable(0x%x): pagetable = 0x%x, index = 0x%x\n", context, pagetable,index);
  context->pagedir[index] = (uint32_t) pagetable | context->flags | VMM_PRESENT;
  
  return vmm_get_pagetable(context, index);
}

vmm_pt_t vmm_get_pagetable(vmm_context_t *context, int index) {
  vmm_pt_t pagetable;
  
  if(paging_enabled) {
    pagetable = (vmm_pt_t) (context->pagedir[index] & PAGE_MASK);
    if(context != current_context) {
      uintptr_t vaddr = vmm_find_free_page(current_context);
      vmm_map_page(current_context, vaddr, pagetable);
      pagetable = vaddr;
    }
  } else {
    pagetable = (vmm_pt_t) (context->pagedir[index] & PAGE_MASK);
  }
  debug(VMM_DEBUG, "vmm_get_pagetable(0x%x): index = 0x%x, pagetable = 0x%x\n", context, index, pagetable);
  
  return pagetable;
}

vmm_context_t *vmm_create_context(uint8_t flags) {
  vmm_context_t *context = pmm_alloc();
  uintptr_t paddr = context;
  
  if(paging_enabled) {
    uintptr_t vaddr = vmm_find_free_page(current_context);
    debug(VMM_DEBUG, "vmm_create_context(): paddr = 0x%x, vaddr = 0x%x\n", paddr, vaddr);
    vmm_map_page(current_context, vaddr, context);
    context = vaddr;
  } else {
    debug(VMM_DEBUG, "vmm_create_context(): paddr = 0x%x\n", context);
  }
  memclr(context, PAGE_SIZE);
  context->flags = flags;
  context->alloc_offset = 1;
  
  // create pagedirectory
  vmm_pd_t pagedir = pmm_alloc();
  uintptr_t pd_paddr = pagedir;
  uintptr_t pd_vaddr = vmm_find_free_page(context);
  if(paging_enabled) {
    uintptr_t vaddr = vmm_find_free_page(current_context);
    vmm_map_page(current_context, vaddr, pagedir);
    pagedir = vaddr;
  }
  memclr(pagedir, PAGE_SIZE);
  pagedir[PD_INDEX(pd_vaddr)] = (uint32_t) pagedir | context->flags | VMM_PRESENT;
  
  context->pagedir = pagedir;
  context->pagedir_paddr = pd_paddr;
  
  vmm_map_page(context, vmm_find_free_page(context), paddr);
  
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
    debug(VMM_DEBUG, "vmm_map_page(0x%x): map now 0x%x to 0x%x\n", context, vaddr, paddr);
    if(context->pagedir[pd_index] & VMM_PRESENT) {
      pagetable = vmm_get_pagetable(context, pd_index);
    } else {
      pagetable = vmm_create_pagetable(context, pd_index);
    }
    
    pagetable[pt_index] = paddr | context->flags | VMM_PRESENT;
    vmm_flush_tlb(vaddr);
    
    return 0;
  }
}

void *vmm_find_free_page(vmm_context_t *context) {
  uintptr_t vaddr;
  vaddr = (uintptr_t) context->alloc_offset * PAGE_SIZE;
  context->alloc_offset++;
  
  return (void*)vaddr;
}

void *vmm_alloc(void) {
  uintptr_t paddr = (uintptr_t) pmm_alloc();
  uintptr_t vaddr = vmm_find_free_page(current_context);
  debug(VMM_DEBUG, "vmm_alloc(0x%x): vaddr = 0x%x, paddr = 0x%x\n", current_context, vaddr, paddr);
  vmm_map_page(current_context, vaddr, paddr);
  
  return (void*) vaddr;
}
