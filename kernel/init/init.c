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
#define _NO_USER_INC
#include <sys/elf.h>
#include <sys/syscalls.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <alloca.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <debug/debug.h>
#include <debug/panic.h>
#include <driver/cmos.h>
#include <driver/console.h>
#include <driver/kbc.h>
#include <driver/keyboard.h>
#include <driver/pit.h>
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
#include <vfs.h>

void init(struct multiboot_info *mb_info) {
  setColor(0x0f);
  clearscreen();
  
  setColor(0x06);
  printf("The Fruity-kernel is starting now...\n");
  setColor(0x07);
  dinip("Initalizing PIT... ");
    init_pit(PIT_FREQ);endini();
  kinip("Initalizing pmm... ");
    init_pmm(mb_info);endini();
  kinip("Initalizing paging... ");
    init_vmm(mb_info);endini();
  kinip("Initalizing GDT... ");
    init_gdt();endini();
  kinip("Initalizing interrupts... ");
    init_idt();init_pic();endini();
  kinip("Initalizing kernel-heap... ");
    init_heap();endini();
  kinip("Initalizing syscalltable... ");
    init_syscalltable();endini();
  dinip("Initalizing terminal... ");
    init_console();endini();
  sti();
  dinip("Initalizing CMOS... ");
    init_cmos();endini();
  dinip("Initalizing keyboard... ");
    init_keyboard();endini();
  cli();
  kinip("Register mm syscalls... ");
    mm_init_syscalls();endini();
  kinip("Initalizing scheduler... ");
    init_scheduler();endini();
  kinip("Initalizing VFS... ");
    init_vfs();endini();
  
  setColor(0x06);
  printf("The kernel is successful started!\n");
  setColor(0x0f);


  int mod_count = mb_info->mbs_mods_count;
  if(mod_count > 0) {
    multiboot_module_t *modules = (multiboot_module_t*) mb_info->mbs_mods_addr;
    
    void *mods[2];
    int i;
    for(i = 0; i < mb_info->mbs_mods_count; i++) {
      size_t len = modules[i].mod_end - modules[i].mod_start;
      size_t pages = (len + PAGE_SIZE) / PAGE_SIZE;
      mods[i] = vmm_automap_kernel_area(current_context, modules[i].mod_start, pages);
    }
    
    printf("Loading initial ramdisk...\n");
    vfs_load_initrd(mods[0]);
    
    if(mod_count > 1) {
      loaded_elf_t *elf = load_elf32(mods[1], vmm_create_context(), "init");
      run_elf32(elf);
    } else {
      printf("error: no init-module found!\n");
    }
  } else {
    printf("error: no modules found!\n");
  }
  setColor(0x07);
  clearscreen();
  sti();
  while(1) {
    printf("%c", getch());
  }
}

