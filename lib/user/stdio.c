/**
 *  lib/user/stdio.c
 *
 *  (C) Copyright 2013 Michael Sippel
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
#include <sys/syscalls.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

inline int putchar(const char c) {
  asm volatile("int $0x30" : : "a" (SYSCALL_PUTC), "b" (c));
  
  return 0;
}

inline int puts(const char *s) {
  int len = 0;
  while(s[len++] != '\0');
  asm volatile("int $0x30" : : "a" (SYSCALL_PUTS), "b" (s), "c" (len));
  
  return len;
}

int printf(char *format, ...) {
  va_list args;
  int i = 1, j;
  
  char buffer[1024];
  
  va_start(args, format);
  i = vsprintf(buffer, format, args);
  va_end(args);
  
  j = puts(buffer);
  
  if(j == i)
    return i;
  else
    return -1;
}

inline char getch(void) {
  char ret = 0;
  asm volatile("int $0x30" : : "a" (SYSCALL_GETC), "b" (ret));
  return ret;
}

inline char *gets(char *s) {
  asm volatile("int $0x30" : : "a" (SYSCALL_GETS), "b" (s));
  
  return s;
}
