/**
 *  test/test.c
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

int i = 0;
void _start(void) {
    int j = i + 5;
    asm volatile("int $0x30" : : "a" (SYSCALL_PUTC), "b" ('0' +i));
    asm volatile("int $0x30" : : "a" (SYSCALL_PUTC), "b" ('\n'));
    for (; i < j; i++) {
        asm volatile("int $0x30" : : "a" (SYSCALL_PUTC), "b" ('0' + i));
    }
    asm volatile("int $0x30" : : "a" (SYSCALL_PUTC), "b" ('\n'));
    while(1);
}
