/**
 *  kernel/mm/vmm.c
 *
 *  (C) Copyright 2012-2013 Michael Sippel
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
#include <interrupt.h>

#define VMM_DEBUG 1

vmm_context_t *current_context;
vmm_context_t *kernel_context;
static int paging_initalized = 0;
static uint32_t cr0;

void init_vmm(multiboot_info_t *mb_info) {
  uintptr_t kernel_context_paddr = (uintptr_t) pmm_alloc();
  kernel_context = (void*) kernel_context_paddr + VADDR_KERNEL_START;
  memclr(kernel_context, PAGE_SIZE);
  
  uintptr_t pagedir_paddr = (uintptr_t) pmm_alloc();
  vmm_pd_t pagedir = (void*) pagedir_paddr + VADDR_KERNEL_START;
  memclr(pagedir, PAGE_SIZE);
  pagedir[PD_INDEX(PAGE_INDEX(VADDR_PT_START))] = (uint32_t) pagedir_paddr | VMM_KERNEL_FLAGS;
  kernel_context->pagedir = pagedir;
  kernel_context->pagedir_paddr = pagedir_paddr;
  
  vmm_map_area(kernel_context, VADDR_KERNEL_START, (uintptr_t) 0, KERNEL_PAGES, VMM_KERNEL_FLAGS);// kernel
  vmm_map_area(kernel_context, VIDEOMEM_START, VIDEOMEM_START, VIDEOMEM_PAGES, VMM_KERNEL_FLAGS);// videomemory (0xB8000 - 0xBFFFF)
  // multiboot
  vmm_map_page(kernel_context, (uintptr_t)mb_info & PAGE_MASK, ((uintptr_t)mb_info& PAGE_MASK) - VADDR_KERNEL_START, VMM_KERNEL_FLAGS); 
  vmm_map_page(kernel_context, mb_info->mbs_mods_addr & PAGE_MASK, mb_info->mbs_mods_addr & PAGE_MASK, VMM_KERNEL_FLAGS);
  int i;
  uintptr_t addr;
  multiboot_module_t *modules = (void*) mb_info->mbs_mods_addr;
  for(i = 0; i < mb_info->mbs_mods_count; i++) {
    addr = modules[i].mod_start & PAGE_MASK;
    while(addr < modules[i].mod_end) {
      vmm_map_page(kernel_context, addr, addr, VMM_KERNEL_FLAGS);
      addr += PAGE_SIZE;
    }
  }
  
  void *pd = vmm_automap_kernel_page(kernel_context, pagedir_paddr);
  void *ct = vmm_automap_kernel_page(kernel_context, kernel_context_paddr);
  kernel_context->pagedir = pd;
  kernel_context = ct;
  
  current_context = kernel_context;
  asm volatile("mov %0, %%cr3" : : "r" (pagedir_paddr));
  paging_initalized = 1;
  
  // pagefault handler
  set_exception_handler(0xe, pagefault);
}

inline void vmm_enable(void) {
  asm volatile("mov %%cr0, %0" : "=r" (cr0));
  cr0 |= (1 << 31);
  asm volatile("mov %0, %%cr0" : : "r" (cr0));
  paging_initalized = 1;
}

inline void vmm_disable(void) {
  asm volatile("mov %%cr0, %0" : "=r" (cr0));
  cr0 &= ~(1 << 31);
  asm volatile("mov %0, %%cr0" : : "r" (cr0));
  paging_initalized = 0;
}

vmm_pt_t vmm_create_pagetable(vmm_context_t *context, int index, uint8_t flags) {
  vmm_pt_t pagetable = pmm_alloc();
  context->pagedir[index] = (uint32_t) pagetable | VMM_WRITE | VMM_PRESENT | flags;
  pagetable = vmm_get_pagetable(context, index, flags);
  memclr(pagetable, PAGE_SIZE);
  
  return pagetable;
}

vmm_pt_t vmm_get_pagetable(vmm_context_t *context, int index, uint8_t flags) {
  vmm_pt_t pagetable;
  
  context->pagedir[index] |= flags;
  
  if(paging_initalized) {
    if(context != current_context) {
      pagetable = (vmm_pt_t) vmm_automap_kernel_page(current_context, (uintptr_t) PT_PADDR(context, index));
    } else {
      pagetable = (vmm_pt_t) PT_VADDR(index);
    }
  } else {
    pagetable = (vmm_pt_t) (PT_PADDR(context, index) + (uintptr_t) VADDR_KERNEL_START);
  }
  
  return pagetable;
}

vmm_context_t *vmm_create_context(void) {
  uintptr_t paddr = (uintptr_t) pmm_alloc();
  vmm_context_t *context = vmm_automap_kernel_page(current_context, paddr);
  memclr(context, PAGE_SIZE);
  
  // create pagedirectory
  uintptr_t pd_paddr = (uintptr_t) pmm_alloc();
  vmm_pd_t pagedir = vmm_automap_kernel_page(current_context, pd_paddr);
  memclr(pagedir, PAGE_SIZE);
  
  context->pagedir = pagedir;
  context->pagedir_paddr = pd_paddr;
  
  vmm_update_context(context);
  pagedir[PD_INDEX(PAGE_INDEX(VADDR_PT_START))] = (uint32_t) pd_paddr | VMM_PRESENT | VMM_WRITE;
  
  return context;
}

vmm_context_t *vmm_fork(vmm_context_t *context) {
  vmm_context_t *new_context = vmm_create_context();
  
  int i, j;
  for(i = 0; i < PD_SIZE; i++) {
    if(context->pagedir[i]) {
      vmm_pt_t *new_pt = vmm_create_pagetable(new_context, i, VMM_USER_FLAGS);
      vmm_pt_t *pt     = vmm_get_pagetable(context, i, VMM_USER_FLAGS);
      memcpy((void*)new_pt, (void*)pt, PAGE_SIZE);
    }
  }
  
  return new_context;
}

inline void vmm_update_context(vmm_context_t *context) {
  #define START PD_INDEX(PAGE_INDEX(VADDR_KERNEL_START))
  #define END   PD_INDEX(PAGE_INDEX(VADDR_KERNEL_END))
  uintptr_t upd = (uintptr_t) (context->pagedir + START);
  uintptr_t kpd = (uintptr_t) (current_context->pagedir + START);
  size_t len = END - START;
  memcpy((void*) upd, (void*) kpd, len * sizeof(vmm_pt_t));
  context->pagedir[PD_INDEX(PAGE_INDEX(VADDR_PT_START))] = (uint32_t) context->pagedir_paddr | VMM_PRESENT | VMM_WRITE;
}

inline void vmm_activate_context(vmm_context_t *context) {
  if(current_context != context && context != NULL) {
    vmm_update_context(context);
    current_context = context;
    asm volatile("mov %0, %%cr3" : : "r" (context->pagedir_paddr));
  }
}

inline void vmm_flush_tlb(uintptr_t vaddr) {
  asm volatile("invlpg %0" : : "m" (*(char*) vaddr));
}

int vmm_map_page(vmm_context_t *context, uintptr_t vaddr, uintptr_t paddr, uint8_t flags) {
  uint32_t page_index = PAGE_INDEX(vaddr);
  uint32_t pd_index = PD_INDEX(page_index);
  uint32_t pt_index = PT_INDEX(page_index);
  vmm_pt_t pagetable;
  
  if( (vaddr & ~PAGE_MASK) || (paddr & ~PAGE_MASK) ) {
    char msg[70];
    sprintf(msg, "vmm_map_page(0x%x): Can't map 0x%x to 0x%x! No 4k alignment!", context, vaddr, paddr);
    debug(VMM_DEBUG, msg);
    return -1;
  } else {
    if(context->pagedir[pd_index] & VMM_PRESENT) {
      pagetable = vmm_get_pagetable(context, pd_index, flags);
    } else {
      pagetable = vmm_create_pagetable(context, pd_index, flags);
    }
    
    pagetable[pt_index] = (uint32_t) paddr | flags | VMM_PRESENT; // entry mapping
    
    if(context == current_context) {
      vmm_flush_tlb(vaddr);
    } else {
      if(paging_initalized) {
	vmm_unmap_page(current_context, (uintptr_t) pagetable);
      }
    }
  }
  
  return 0;
}

int vmm_unmap_page(vmm_context_t *context, uintptr_t vaddr) {
  uint32_t page_index = PAGE_INDEX(vaddr);
  vmm_pt_t pt = vmm_get_pagetable(context, PD_INDEX(page_index), 0);
  pt[PT_INDEX(page_index)] = 0;
  
  int i = 0;
  while(i < PT_SIZE) {
    if(pt[i] != 0) {
      break;
    } else {
      if(i == PT_SIZE-1) {
	context->pagedir[PD_INDEX(page_index)] = 0;
	pmm_free(pt);
      } else {
	i++;
      }
    }
  }
  
  return 0;
}

int vmm_map_area(vmm_context_t *context, uintptr_t vaddr, uintptr_t paddr, size_t pages, uint8_t flags) {
  int page;
  for(page = 0; page < pages; page++) {
    vmm_map_page(context, vaddr + page*PAGE_SIZE, paddr + page*PAGE_SIZE, flags);
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
  #define PAGES_FOUND(l) \
    if(vaddr == (uintptr_t)NULL) { \
      page = pd_index * PT_SIZE + pt_index; \
      vaddr = page * PAGE_SIZE; \
    } \
    pages_found += l; \
    if(pages_found >= num) { \
      return (void*) vaddr; \
    }
  
  uintptr_t vaddr = (uintptr_t) NULL;
  uintptr_t page = 0;
  size_t pages_found = 0;
  
  uint32_t pd_index = PD_INDEX(PAGE_INDEX(limit_low));
  uint32_t pt_index = PT_INDEX(PAGE_INDEX(limit_low));
  uint32_t pd_index_end = PD_INDEX(PAGE_INDEX(limit_high));
  uint32_t pt_index_end = PT_INDEX(PAGE_INDEX(limit_high));
  vmm_pt_t pt;
  
  while(pd_index <= pd_index_end) {
    if(context->pagedir[pd_index] & VMM_PRESENT) {
      pt = vmm_get_pagetable(context, pd_index, VMM_KERNEL_FLAGS);
      
      uint32_t pt_end = (pd_index == pd_index_end) ? pt_index_end : PT_SIZE; // last pd entry
      for(;pt_index < pt_end; pt_index++) {
        if(! ((uint32_t)pt[pt_index] & VMM_PRESENT) ) {
          PAGES_FOUND(1);
        } else {
          pages_found = 0;
          vaddr = (uintptr_t)NULL;
        }
      }
      pt_index = 0;
    } else {
      PAGES_FOUND(PT_SIZE);
    }
    pd_index++;
  }
  
  return NULL;
}

void *vmm_automap_kernel_page(vmm_context_t *context, uintptr_t paddr) {
  uintptr_t vaddr = (uintptr_t) vmm_find(context, 1, VADDR_KERNEL_START, VADDR_KERNEL_END);
  vmm_map_page(context, vaddr, paddr, VMM_KERNEL_FLAGS);
  
  return (void*) vaddr;
}

void *vmm_automap_kernel_area(vmm_context_t *context, uintptr_t paddr, size_t pages) {
  uintptr_t vaddr = (uintptr_t) vmm_find(context, pages, VADDR_KERNEL_START, VADDR_KERNEL_END);
  vmm_map_area(context, vaddr, paddr, pages, VMM_KERNEL_FLAGS);
  
  return (void*) vaddr;
}

void *vmm_automap_user_page(vmm_context_t *context, uintptr_t paddr) {
  uintptr_t vaddr = (uintptr_t) vmm_find(context, 1, VADDR_USER_START, VADDR_USER_END);
  vmm_map_page(context, vaddr, paddr, VMM_USER_FLAGS);
  
  return (void*) vaddr;
}

void *vmm_automap_user_area(vmm_context_t *context, uintptr_t paddr, size_t pages) {
  uintptr_t vaddr = (uintptr_t) vmm_find(context, pages, VADDR_USER_START, VADDR_USER_END);
  vmm_map_area(context, vaddr, paddr, pages, VMM_USER_FLAGS);
  
  return (void*) vaddr;
}

void *vmm_alloc(void) {
  uintptr_t paddr = (uintptr_t) pmm_alloc();
  uintptr_t vaddr = (uintptr_t) vmm_find(current_context, 1, VADDR_USER_START, VADDR_USER_END);
  vmm_map_page(current_context, vaddr, paddr, VMM_USER_FLAGS);
  
  return (void*) vaddr;
}

void *vmm_alloc_area(size_t pages) {
  uintptr_t paddr;
  uintptr_t vaddr_start = (uintptr_t) vmm_find(current_context, pages, VADDR_USER_START, VADDR_USER_END);
  uintptr_t vaddr = vaddr_start;
  
  int i;
  for(i = 0; i < pages; i++) {
    paddr = (uintptr_t) pmm_alloc();
    vmm_map_page(current_context, vaddr, paddr, VMM_USER_FLAGS);
    vaddr += PAGE_SIZE;
  }
  
  return (void*) vaddr_start;
}

uintptr_t vmm_paddr(vmm_context_t *context, uintptr_t vaddr) {
  uintptr_t paddr;
  uint32_t page_index = PAGE_INDEX(vaddr);
  uint32_t pd_index = PD_INDEX(page_index);
  uint32_t pt_index = PT_INDEX(page_index);
  
  vmm_pt_t pt = vmm_get_pagetable(context, pd_index, 0);
  paddr = (uintptr_t) pt[pt_index] & PAGE_MASK;
  
  return paddr;
}

