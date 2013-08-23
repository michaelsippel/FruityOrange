/**
 *  kernel/proc/elf32.c
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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <sys/elf.h>
#include <stdint.h>
#include <string.h>

#include <driver/console.h>
#include <proc/proc.h>

loaded_elf_t *load_elf32(void *image, vmm_context_t *context, const char *name) {
  elf32_header_t *header = image;
  elf32_program_header_t *ph;
  int i, j;
  size_t pages;
  
  // check ELF-Magic
  if(header->ident[EI_MAG0] == ELF_MAG0 &&
     header->ident[EI_MAG1] == ELF_MAG1 &&
     header->ident[EI_MAG2] == ELF_MAG2 &&
     header->ident[EI_MAG3] == ELF_MAG3) 
  {
    if(header->ident[4] == 1) {
      if(header->ident[5] == 1) {
	if(header->machine == EM_386) {
	  if(header->type == ET_EXEC) {
	    if(header->version == ELF_VERSION_CURRENT) {
	      // ELF o.k.!
	    } else {
	      printf("[elf32] Invalid ELF-version!\n");
	      return NULL;
	    }
	  } else {
	    printf("[elf32] ELF-Type isn't executable!\n");
	    return NULL;
	  }
	} else {
	  printf("[elf32] Invalid machine!\n");
	  return NULL;
	}
      } else {
	printf("[elf32] Invalid byte-encoding!\n");
	return NULL;
      }
    } else {
      printf("[elf32] Invalid architecture!\n");
      return NULL;
    }
  } else {
    printf("[elf32] Invalid ELF-Magic! (0x%4x)\n", *header);
    printf("[elf32] image at 0x%x\n", image);
    return NULL;
  }
  
  ph = (elf32_program_header_t*) (((uintptr_t) image) + header->ph_offset);
  for(i = 0; i < header->ph_entry_count; i++, ph++) {
    if(ph->type == EPT_LOAD) {
      pages = NUM_PAGES(ph->mem_size);
      uintptr_t dest = (uintptr_t) vmm_find(current_context, 1, VADDR_KERNEL_START, VADDR_KERNEL_END);
      
      for(j = 0; j < pages; j++) {
	uintptr_t paddr = (uintptr_t) pmm_alloc();
	uintptr_t vaddr = (uintptr_t) ph->virt_addr + j*PAGE_SIZE;
	uintptr_t src   = (uintptr_t) image + ph->offset + j*PAGE_SIZE;
	
	vmm_map_page(context, vaddr, paddr, VMM_USER_FLAGS);
	vmm_map_page(current_context, dest, paddr, VMM_USER_FLAGS);
	
	memcpy((void*) dest, (void*) src, PAGE_SIZE);
      }
      
      memclr((void*) dest + ph->mem_size - pages*PAGE_SIZE, ph->mem_size - ph->file_size);
      vmm_unmap_page(context, dest);
    }
  }
  
  loaded_elf_t *elf = malloc(sizeof(loaded_elf_t));
  elf->entry = (void*) header->entry;
  elf->name = name;
  elf->context = context;
  elf->dpl = DPL_USERMODE;
  
  return elf;
}

proc_t *run_elf32(loaded_elf_t *elf) {
  return create_proc(elf->entry, elf->name, elf->context, elf->dpl);
}

