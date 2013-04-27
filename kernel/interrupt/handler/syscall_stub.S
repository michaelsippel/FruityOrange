//
//  kernel/interrupt/handler/syscall.S
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

.extern handle_syscall
.extern common_eoi

.global int_handler0x30
int_handler0x30:
  cli
  push $0
  push $0x30
  
  push %gs
  push %fs
  push %es
  push %ds
  
  push %ebp
  push %edi
  push %esi
  push %edx
  push %ecx
  push %ebx
  push %eax
  
  // load  kernel-datasegments
  mov $0x10, %ax
  mov %ax, %ds
  mov %ax, %es
  mov %ax, %fs
  mov %ax, %gs
  
  push %esp
  call handle_syscall
  mov %eax, %esp
  
  pop %eax
  pop %ebx
  pop %ecx
  pop %edx
  pop %esi
  pop %edi
  pop %ebp
  
  pop %ds
  pop %es
  pop %fs
  pop %gs
  
  sti
  // interrupt number is already at the stack
  call common_eoi
  add $8, %esp
  iret
