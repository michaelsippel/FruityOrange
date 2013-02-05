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
  pagedir[PD_INDEX(PAGE_INDEX(VADDR_PT_START))] = (uint32_t) pagedir | VMM_KERNEL_FLAGS;
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
/*    
   //TODO: let this work: 
   vmm_map_area(kernel_context, 0x0, 0x0, KERNEL_PAGES);// until kernel_end 1:1 mapping
   vmm_map_area(kernel_context, VADDR_KERNEL_START, (uintptr_t) &kernel_start, KERNEL_PAGES);// kernel
   vmm_map_area(kernel_context, VIDEOMEM_START, VIDEOMEM_START, VIDEOMEM_PAGES);// videomemory (0xB8000 - 0xBFFFF)
  */
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

vmm_pt_t vmm_create_pagetable(vmm_context_t *context, int index) {
  vmm_pt_t pagetable = pmm_alloc();
  context->pagedir[index] = (uint32_t) pagetable | VMM_WRITE | VMM_PRESENT;
  vmm_map_page(context, PT_VADDR(index), (uintptr_t)pagetable);
  
  pagetable = vmm_get_pagetable(context, index);
  memclr(pagetable, PAGE_SIZE);
  
  return pagetable;
}

vmm_pt_t vmm_get_pagetable(vmm_context_t *context, int index) {
  static uintptr_t vaddr_pt = 0;
  static uintptr_t vaddr_pd = 0;
  vmm_pt_t pagetable;
  
  // TODO (waiting for vmm_map_temp())
//   if(! context->pagedir[index] & VMM_PRESENT) {
//     return NULL;
//   }
  
  if(paging_enabled) {
    if(context != current_context) {
      if(vaddr_pd == 0)
	vaddr_pd = (uintptr_t) vmm_find(current_context, 1, VADDR_KERNEL_START, VADDR_KERNEL_END);
      if(vaddr_pt == 0)
	vaddr_pt = (uintptr_t) vmm_find(current_context, 1, VADDR_KERNEL_START, VADDR_KERNEL_END);
      
      printf("vaddr_pd = 0x%x\n", context->pagedir_paddr);
      vmm_map_page(current_context, vaddr_pd, context->pagedir_paddr);
      context->pagedir = vaddr_pd;
      
      vmm_map_page(current_context, vaddr_pt, (uintptr_t) PT_PADDR(context, index));
      pagetable = (vmm_pt_t) vaddr_pt;
    } else {
      pagetable = (vmm_pt_t) PT_VADDR(index);
    }
  } else {
    pagetable = (vmm_pt_t) PT_PADDR(context, index);
  }
  
  return pagetable;
}

vmm_context_t *vmm_create_context(uint8_t flags) {
  uintptr_t paddr = (uintptr_t) pmm_alloc();
  vmm_context_t *context = vmm_automap_kernel_page(current_context, paddr);
  
  memclr(context, PAGE_SIZE);
  context->flags = flags;
  context->alloc_offset = 1;
  
  // create pagedirectory
  uintptr_t pd_paddr = (uintptr_t) pmm_alloc();
  vmm_pd_t pagedir = vmm_automap_kernel_page(current_context, pd_paddr);
  memclr(pagedir, PAGE_SIZE);
  
  context->pagedir = pagedir;
  context->pagedir_paddr = pd_paddr;
  
  // copy kernelmappings
//   memcpy(context->pagedir, kernel_context->pagedir, 0xff * sizeof(uint32_t));
  pagedir[PD_INDEX(PAGE_INDEX(VADDR_PD))] = (uint32_t) pd_paddr | context->flags;
  printf("0x%x\n", context->pagedir_paddr);
  vmm_map_page(context, VADDR_CONTEXT, paddr);
  vmm_map_page(context, VADDR_PD, pd_paddr);
  
  return context;
}

inline void vmm_activate_context(vmm_context_t *context) {
  if(current_context != context || current_context == NULL) {
    asm volatile("mov %0, %%cr3" : : "r" (context->pagedir_paddr));
    context = VADDR_CONTEXT;
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

int vmm_unmap_page(vmm_context_t *context, uintptr_t vaddr) {
  return vmm_map_page(context, vaddr, (uintptr_t) NULL);
}

int vmm_map_area(vmm_context_t *context, uintptr_t vaddr, uintptr_t paddr, size_t pages) {
  int page;
  for(page = 0; page < pages; page++) {
    vmm_map_page(context, vaddr + page*PAGE_SIZE, paddr +page*PAGE_SIZE);
  }
  return 0;
}

int vmm_unmap_area(vmm_context_t *context, uintptr_t vaddr, size_t pages) {
  int page;
  for(page = 0; page < pages; page++) {
    vmm_unmap_page(context, vaddr + page*PAGE_SIZE);
  }
  return 0;
}

void *vmm_find(vmm_context_t *context, size_t num, uintptr_t limit_low, uintptr_t limit_high) {
  uintptr_t vaddr;
  uintptr_t page;
  size_t pages_found = 0;
  
  uint32_t pd_index = PD_INDEX(PAGE_INDEX(limit_low));
  uint32_t pt_index = PT_INDEX(PAGE_INDEX(limit_low));
  uint32_t pd_index_end = PD_INDEX(PAGE_INDEX(limit_high));
  uint32_t pt_index_end = PT_INDEX(PAGE_INDEX(limit_high));
  vmm_pt_t pt;
  
  while(pd_index <= pd_index_end) {
    if(context->pagedir[pd_index] & VMM_PRESENT) {
      pt = vmm_get_pagetable(context, pd_index);
      
      uint32_t end = (pd_index == pd_index_end) ? pt_index_end : PT_SIZE; // last pd entry
      while(pt_index < end) {
	if(! ((uint32_t)pt[pt_index++] & VMM_PRESENT) ) {
	  if(vaddr != (uintptr_t)NULL) {
	    page = pd_index * PT_SIZE + pt_index;
	    vaddr = page * PAGE_SIZE;
	  }
	  pages_found ++;
	  if(pages_found >= num) {
	    return (void*) vaddr;
	  }
	}
      }
      pt_index = 0;
    } else {
      if(vaddr != (uintptr_t) NULL) {
	page = pd_index * PT_SIZE + pt_index;
	vaddr = page * PAGE_SIZE;
      }
      pages_found += PT_SIZE;
      if(pages_found >= num) {
	return (void*) vaddr;
      }
    }
    pd_index++;
  }
  
  return NULL;
}

void *vmm_automap_kernel_page(vmm_context_t *context, uintptr_t paddr) {
  uintptr_t vaddr = (uintptr_t) vmm_find(context, 1, VADDR_KERNEL_START, VADDR_KERNEL_END);
  vmm_map_page(context, vaddr, paddr);
  
  return (void*) vaddr;
}

void *vmm_automap_kernel_area(vmm_context_t *context, uintptr_t paddr, size_t pages) {
  uintptr_t vaddr = (uintptr_t) vmm_find(context, pages, VADDR_KERNEL_START, VADDR_KERNEL_END);
  vmm_map_area(context, vaddr, paddr, pages);
  
  return (void*) vaddr;
}

void *vmm_automap_user_page(vmm_context_t *context, uintptr_t paddr) {
  uintptr_t vaddr = (uintptr_t) vmm_find(context, 1, VADDR_USER_START, VADDR_USER_END);
  vmm_map_page(context, vaddr, paddr);
  
  return (void*) vaddr;
}

void *vmm_automap_user_area(vmm_context_t *context, uintptr_t paddr, size_t pages) {
  uintptr_t vaddr = (uintptr_t) vmm_find(context, pages, VADDR_USER_START, VADDR_USER_END);
  vmm_map_area(context, vaddr, paddr, pages);
  
  return (void*) vaddr;
}

void *vmm_alloc(void) {
  uintptr_t paddr = (uintptr_t) pmm_alloc();
  uintptr_t vaddr = (uintptr_t) vmm_find(current_context, 1, VADDR_USER_START, VADDR_USER_END);
  vmm_map_page(current_context, vaddr, paddr);
  
  return (void*) vaddr;
}

void *vmm_alloc_area(size_t pages) {
  uintptr_t paddr;
  uintptr_t vaddr_start = (uintptr_t) vmm_find(current_context, pages, VADDR_USER_START, VADDR_USER_END);
  uintptr_t vaddr = vaddr_start;
  while(pages--) {
    paddr = (uintptr_t) pmm_alloc();
    vmm_map_page(current_context, vaddr, paddr);
    vaddr += PAGE_SIZE;
  }
  
  return (void*) vaddr_start;
}

uintptr_t vmm_paddr(vmm_context_t *context, uintptr_t vaddr) {
  uintptr_t paddr;
  uint32_t page_index = PAGE_INDEX(vaddr);
  uint32_t pd_index = PD_INDEX(page_index);
  uint32_t pt_index = PT_INDEX(page_index);
  
  vmm_pt_t pt = vmm_get_pagetable(context, pd_index);
  paddr = (uintptr_t) pt[pt_index] & PAGE_MASK;
  
  return paddr;
}
