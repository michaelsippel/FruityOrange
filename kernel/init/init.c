/**
 *  kernel/init.c
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
#include <sys/types.h>
#include <alloca.h>
#include <elf.h>
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

// TODO: move!
void syscall_putc(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  printf("%c", *ebx);
}

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
  kinip("Initalizing keyboard... ");
    init_keyboard();endini();
  
  setup_syscall(0, "putchar", &syscall_putc);// TODO: move!
    
  setColor(0x06);
  printf("The kernel is successful started!\n");
  setColor(0x0f);
  
  // TODO!!!!
  struct multiboot_module *modules = mb_info->mbs_mods_addr;
  size_t length = modules[0].mod_end - modules[0].mod_start;
  void *load_addr = (void*) 0x200000;
  
  vmm_map_page(current_context, load_addr, modules[0].mod_start);
  create_proc(load_addr, length,"", DPL_KERNELMODE);
  
  while(1) {
    printf("%c", getch());
  }
}
