//
//  kernel/boot.S
//
//  (C) Copyright 2012-2013 Michael Sippel
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
#define VADDR_KERNEL_START 0xC0000000
#define KERNEL_PAGES (VADDR_KERNEL_START >> 22)

.section multiboot
#define MB_MAGIC 0x1BADB002
#define MB_FLAGS 0x0
#define MB_CHECKSUM -(MB_MAGIC + MB_FLAGS)

.align 4
.int     MB_MAGIC
.int     MB_FLAGS
.int     MB_CHECKSUM

.section .data
.align 0x1000
_first_pd:
    .long 0x00000083

    .rept (KERNEL_PAGES - 1)  
    .long 0
    .endr

    .long 0x00000083

    .rept (1024 - KERNEL_PAGES - 1)
    .long 0
    .endr

.section .text
.global _start
.extern init
_start:
  movl $_first_pd - VADDR_KERNEL_START, %ecx
  movl %ecx, %cr3
  
  movl %cr4, %ecx
  orl $(1 << 4), %ecx
  movl %ecx, %cr4
  
  movl %cr0, %ecx
  orl $(1 << 31), %ecx
  movl %ecx, %cr0
  
  lea (_higherhalf), %ecx
  jmp %ecx

_higherhalf: 
  movl $kernel_stack, %esp
  addl $VADDR_KERNEL_START, %ebx
  pushl %ebx
  call init
  jmp _stop

_stop:
  cli
  hlt
  jmp _stop

.section .bss
.space 8192

kernel_stack:

