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
#include <alloca.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <driver/kbc.h>
#include <driver/keyboard.h>
#include <console.h>
#include <debug.h>
#include <gdt.h>
#include <interrupt.h>
#include <mm.h>
#include <multiboot.h>
#include <panic.h>
#include <portio.h>

void init(struct multiboot_info *mb_info) {
  clearscreen();
  
  setColor(0x06);
  printf("The OrangePalm kernel is starting now...\n");
  setColor(0x07);
  kinip("Initalizing pmm... ");
    init_pmm(mb_info);endini();
//   kinip("Initalizing paging... ");
//     init_vmm();endini();
  kinip("Initalizing GDT... ");
    init_gdt();endini();
  kinip("Initalizing interrupts... ");
    init_idt();init_pic();sti();endini();
  kinip("Initalizing keyboard... ");
    init_keyboard();endini();

  setColor(0x06);
  printf("The kernel is successfull started!\n");
  
  setColor(0x0f);
  while(1) {
    printf("%c", getch());
  }
}
