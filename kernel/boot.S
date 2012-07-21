//
//  kernel/boot.S
//
//  (C) Copyright 2012 Michael Sippel
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

.section .multiboot
#define MB_MAGIC 0x1BADB002
#define MB_FLAGS 0x0
#define MB_CHECKSUM -(MB_MAGIC + MB_FLAGS)

.align 4
.int     MB_MAGIC
.int     MB_FLAGS
.int     MB_CHECKSUM

.section .text
.extern init

.global _start
_start:
    mov $kernel_stack, %esp
    call init
    jmp _stop

_stop:
    cli
    hlt
    jmp _stop

.section .bss
.space 8192
kernel_stack:
