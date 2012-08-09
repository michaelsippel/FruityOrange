/**
 *  kernel/mm/pmm.c
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
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <multiboot.h>
#include <mm.h>

#define BITMAP_SIZE 0x8000
uint32_t bitmap[BITMAP_SIZE];
uint32_t last_free_page = 0;
bool can_do_fast_alloc = FALSE;

void pmm_init(struct multiboot_info *mb_info) {
  int i;
  uintptr_t addr;
  
  // 1. occupy complete memory
  for(i = 0;i < BITMAP_SIZE; i++) {
    bitmap[i] = 0;
  }
  
  // 2. release the pages, which are marked in BIOS as free
  struct multiboot_mmap *mmap = mb_info->mbs_mmap_addr;
  struct multiboot_mmap *mmap_end = (void*) ((uintptr_t) mb_info->mbs_mmap_addr +
                                                         mb_info->mbs_mmap_length);
  while(mmap < mmap_end) {
    if(mmap->type == PMM_FREE) {
      addr = mmap->base;
      uintptr_t end_addr = addr + mmap->length;
      
      while(addr < end_addr) {
	pmm_free((void*) addr);
	addr += PAGE_SIZE;
      }
    }
    mmap++;
  }
  
  // 3. occupy the kernel
  addr = (uintptr_t) &kernel_start;
  while(addr < (uintptr_t) &kernel_end) {
    pmm_mark_used((void*) addr);
    addr += PAGE_SIZE;
  }
  
  // 4. occupy the multiboot-struct
  struct multiboot_module *modules = mb_info->mbs_mods_addr;
  pmm_mark_used(mb_info);
  pmm_mark_used(modules);
  
  // 5. the multiboot-modules too.
  for(i = 0; i < mb_info->mbs_mods_count; i++) {
    addr = modules[i].mod_start;
    while(addr < modules[i].mod_end) {
      pmm_mark_used((void*) addr);
      addr += 0x1000;
    }
  }
}

void* pmm_alloc(void) {
  int i, j, k=0;
  if(can_do_fast_alloc) {
    can_do_fast_alloc = FALSE;
    i = last_free_page / sizeof(uint32_t);
    j = last_free_page % sizeof(uint32_t);
    
    bitmap[i] &= ~(1 << j);
    return last_free_page * PAGE_SIZE;
  } else {
    for(i = 0;i < BITMAP_SIZE; i++) {
      for(j = 0;j < sizeof(uint32_t); j++,k++) {
	if(bitmap[i] & (1 << j)) {
	  bitmap[i] &= ~(1 << j);
	  return (uint32_t) k * PAGE_SIZE;
	}
      }
    }
  }
  return NULL;
}

void pmm_free(void *ptr) {
  uint32_t page = (uint32_t) ptr / PAGE_SIZE;
  
  int i = page / sizeof(uint32_t);
  int j = page % sizeof(uint32_t);
  
  bitmap[i] |= (1 << j);
  last_free_page = page;
  can_do_fast_alloc = TRUE;
}

void pmm_mark_used(void *ptr) {
  uint32_t page = (uint32_t) ptr / PAGE_SIZE;
  
  int i = page / sizeof(uint32_t);
  int j = page % sizeof(uint32_t);
  
  bitmap[i] &= ~(1 << j);
}

