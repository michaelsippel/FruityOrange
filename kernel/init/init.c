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
  kinip("Initalizing pmm... ");
    init_pmm(mb_info);endini();
  kinip("Initalizing paging... ");
    init_vmm();endini();
  kinip("Initalizing GDT... ");
    init_gdt();endini();
  kinip("Initalizing interrupts... ");
    init_idt();init_pic();endini();
  kinip("Initalizing kernel-heap... ");
    init_heap();endini();
  kinip("Initalizing syscalltable... ");
    init_syscalltable();endini();
  kinip("Register mm syscalls... ");
    mm_init_syscalls();endini();
  kinip("Initalizing scheduler... ");
    init_scheduler();endini();
  kinip("Initalizing VFS... ");
    init_vfs();endini();
  
  sti();
  dinip("Initalizing terminal... ");
    init_console();endini();
  dinip("Initalizing PIT... ");
    init_pit(PIT_FREQ);endini();
  dinip("Initalizing CMOS... ");
    init_cmos();endini();
  dinip("Initalizing keyboard... ");
    init_keyboard();endini();
  cli();
  
  cmos_time_t *time = get_cmos_time();
  tm_t tm = mktm(time);
  time_t time_stamp = mktime(tm);
  tm_t new_tm = gmtime(time_stamp);
  
  printf("\nTimestamp: %d\n", time_stamp);
  
  printf("Date: %d/%d/%d  ", new_tm.mday, new_tm.mon, new_tm.year);
  printf("Time: %d:%d:%d (UTC)\n", new_tm.hour, new_tm.min, new_tm.sec);
  
  setColor(0x06);
  printf("The kernel is successful started!\n");
  setColor(0x0f);
  
  if(mb_info->mbs_mods_count > 0) {
    multiboot_module_t *modules = (multiboot_module_t*) mb_info->mbs_mods_addr;
    
    size_t len = modules[0].mod_end - modules[0].mod_start;
    size_t pages = (len + PAGE_SIZE) / PAGE_SIZE;
    char *mod = vmm_automap_kernel_area(current_context, modules[0].mod_start, pages);
    vfs_inode_t *module = vfs_create_inode("initrd.img", S_IWUSR | S_IRUSR | S_IXUSR | S_IXGRP | S_IRGRP, vfs_root());
    module->base = mod;
    module->length = len;
    
    printf("\n");
    vfs_load_initrd(mod);
    
    vfs_inode_list(vfs_root());
  } else {
    printf("error: no modules found!\n");
  }
  printf("\n");
  sti();
  while(1) {
    printf("%c", getch());
  }
}
