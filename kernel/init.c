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

#include <console.h>
#include <debug.h>
#include <gdt.h>
#include <interrupt.h>
#include <panic.h>
#include <portio.h>

void init(void) {
  clearscreen();
  
  setColor(0x06);
  printf("Hello in the OrangePalm World!\n\n");
  setColor(0x0f);
  kinip("Initalizing GDT... ");
    init_gdt();endini();
  kinip("Initaliting Interrupts... ");
    init_idt();endini();
  
  asm volatile("int $0x0");
  while(1);
}
