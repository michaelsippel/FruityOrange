/**
 *  kernel/include/driver/console.h
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
#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <sys/types.h>
#include <stdint.h>
#include <iostream.h>
#include <mm.h>

#define VIDEOMEM_START 0xB8000
#define VIDEOMEM_END   0xBFFFF
#define VIDEOMEM_SIZE  (VIDEOMEM_END - VIDEOMEM_START)
#define VIDEOMEM_PAGES (VIDEOMEM_SIZE / PAGE_SIZE +1)

typedef struct vterm {
  char *name;
  id_t id;
  iostream_t *iostream;
} vterm_t;

void init_console(void);

int putchar(char chr);
int puts(const char *str);
int printf(const char *format, ...);
void scroll(void);
void setCursor(int x, int y);
void setColor(uint8_t ncolor);
uint8_t getColor(void);
void setForegroundColor(uint8_t fcolor);
void setBackgroundColor(uint8_t bcolor);
void clearscreen(void);

void putc_syscall_wrapper(uint32_t *ebx, uint32_t *ecx, uint32_t *edx);
void puts_syscall_wrapper(uint32_t *ebx, uint32_t *ecx, uint32_t *edx);

#endif
