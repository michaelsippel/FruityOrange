/**
 *  lib/user/stdlib/exit.c
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
#include <stdlib.h>

void putenv(char *name, char *value) {
  asm volatile("int $0x30" : : "a" (SYSCALL_PUTENV), "b" (name), "c" (value));
}

char *getenv(const char *name) {
  char buf[100];
  asm volatile("int $0x30" : : "a" (SYSCALL_GETENV), "b" (name), "c" (&buf));
  return buf;
}

