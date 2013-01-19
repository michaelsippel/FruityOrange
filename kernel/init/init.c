/**
 *  kernel/init.c
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
#include <sys/syscalls.h>
#include <sys/types.h>
#include <alloca.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <debug/debug.h>
#include <debug/panic.h>
#include <driver/kbc.h>
#include <driver/keyboard.h>
#include <driver/console.h>
#include <init/gdt.h>
#include <proc/scheduler.h>
#include <proc/proc.h>
#include <proc/task.h>
#include <proc/thread.h>
#include <interrupt.h>
#include <mm.h>
#include <multiboot.h>
#include <syscall.h>
#include <portio.h>

void init(struct multiboot_info *mb_info) {
  setColor(0x0f);
  clearscreen();
  
  setColor(0x06);
  printf("The Fruity-kernel is starting now...\n");
  setColor(0x07);
  kinip("Initalizing pmm... ");
    init_pmm(mb_info);endini();
  kinip("Initalizing paging... ");
    init_vmm();endini();
  kinip("Initalizing GDT... ");
    init_gdt();endini();
  kinip("Initalizing interrupts... ");
    init_idt();init_pic();sti();endini();
  kinip("Initalizing kernel-heap... ");
    init_heap();endini();
  kinip("Initalizing scheduler... ");
    init_scheduler();endini();
  kinip("Initalizing syscalltable... ");
    init_syscalltable();endini();
  dinip("Initalizing terminal... ");
    init_console();endini();
  dinip("Initalizing keyboard... ");
    init_keyboard();endini();
  
  setColor(0x06);
  printf("The kernel is successful started!\n");
  setColor(0x0f);
  
  if(mb_info->mbs_mods_count > 0) {
    if(mb_info->mbs_mods_count > 1) {
      printf("Load %d modules...\n", mb_info->mbs_mods_count);
    } else {
      printf("Load the one module...\n");
    }
    cli();
    struct multiboot_module *modules = mb_info->mbs_mods_addr;
    
    int i;
    for(i = 0; i < mb_info->mbs_mods_count; i++)  {
      size_t pages = (modules[i].mod_end - modules[i].mod_start) / PAGE_SIZE +1;
      void *mod = vmm_automap_area(current_context, modules[i].mod_start, pages);
// 	 vmm_map_area(current_context, mod, modules[i].mod_start, pages);
//       vmm_context_t *new_context = vmm_create_context(VMM_USER_FLAGS);
//       uintptr_t vaddr_new = vmm_automap_area(new_context, modules[i].mod_start, pages);
      load_elf32(mod, modules[i].mod_start, current_context, modules[i].string);
    }
    sti();
  } else {
    printf("error: no modules found!\n");
  }
  
  while(1) {
    printf("%c", getch());
  }
}
