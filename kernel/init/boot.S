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

.section multiboot
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
  lgdt (_first_gdt) - 0xC0000000
  mov $0x10, %ax
  mov %ax, %ds
  mov %ax, %es
  mov %ax, %fs
  mov %ax, %gs
  mov %ax, %ss
  ljmp $0x08, $_higherhalf

_higherhalf:
    mov $kernel_stack, %esp
    push %ebx
    call init
    jmp _stop

_stop:
    cli
    hlt
    jmp _stop

_first_gdt:
    .short _first_gdt_end - _first_gdt_start - 1
    .long _first_gdt_start - 0xC0000000

_first_gdt_start:
    .long 0, 0
    .byte 0xFF, 0xFF, 0, 0, 0, 0b10011010, 0b11001111, 0x40
    .byte 0xFF, 0xFF, 0, 0, 0, 0b10010010, 0b11001111, 0x40

_first_gdt_end:


.section .bss
.space 8192
kernel_stack:
