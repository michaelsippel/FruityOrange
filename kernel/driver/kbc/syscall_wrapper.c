/**
 *  kernel/driver/kbc/syscall_wrapper.c
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
#include <sys/syscalls.h>
#include <stdint.h>

#include <driver/keyboard.h>

void getc_syscall_wrapper(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  *ebx = (uint32_t) getch();
}

void gets_syscall_wrapper(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  *ecx = gets(ebx);
}
