/**
 *  kernel/proc/elf.c
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

proc_t *load_elf32(void *image, void *paddr_img, vmm_context_t *context, const char *name) {
  struct elf32_header *header = image;
  struct elf32_program_header *ph;
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
	      return;
	    }
	  } else {
	    printf("[elf32] ELF-Type isn't executable!\n");
	    return;
	  }
	} else {
	  printf("[elf32] Invalid machine!\n");
	  return;
	}
      } else {
	printf("[elf32] Invalid byte-encoding!\n");
	return;
      }
    } else {
      printf("[elf32] Invalid architecture!\n");
      return;
    }
  } else {
    printf("[elf32] Invalid ELF-Magic! (0x%4x)\n", *header);
    printf("[elf32] image at 0x%x\n", image);
    return;
  }
  
  ph = (struct elf_program_header*) (((char*) image) + header->ph_offset);
  for(i = 0; i < header->ph_entry_count; i++, ph++) {
    if(ph->type == EPT_LOAD) {
      
      int pages = ph->file_size / PAGE_SIZE +1;
      uintptr_t vaddr_start = ph->virt_addr;
      uintptr_t cur_vaddr_start = vmm_find_free_area(current_context, pages);
      uintptr_t cur_img_vaddr_start = vmm_find_free_area(current_context, pages);
      uintptr_t cur_img_paddr_start = ((char*) paddr_img) + ph->offset;
      
      for(j = 0; j < pages; j++) {
	uintptr_t paddr = pmm_alloc();
	uintptr_t vaddr = vaddr_start + j*PAGE_SIZE;
	
	uintptr_t cur_vaddr = cur_vaddr_start + j*PAGE_SIZE;
	uintptr_t cur_img_vaddr = cur_img_vaddr_start + j*PAGE_SIZE;
	uintptr_t cur_img_paddr = cur_img_paddr_start + j*PAGE_SIZE;
	
	vmm_map_page(context, vaddr, paddr);
	vmm_map_page(current_context, cur_vaddr, paddr);
	vmm_map_page(current_context, cur_img_vaddr, cur_img_paddr);
	
	memcpy(cur_vaddr, cur_img_vaddr, PAGE_SIZE);
      }
    }
  }
  
  vmm_map_area(context, header->entry, header->entry, ph->file_size);
  proc_t *proc = create_proc((void*) header->entry, name, context, DPL_KERNELMODE);
  
  return proc;
}
