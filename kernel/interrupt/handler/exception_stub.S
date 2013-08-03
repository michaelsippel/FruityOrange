//
//  kernel/interrupt/handler/exception.S
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
.extern common_eoi
.extern handle_exception

.macro exc_stub_nrm nr
.global int_handler\nr
int_handler\nr:
    push $0
    push $\nr
    jmp common_exc_handler
.endm

.macro exc_stub_err nr
.global int_handler\nr
int_handler\nr:
    push $\nr
    jmp common_exc_handler
.endm

exc_stub_nrm 0x00 //Divide by Zero
exc_stub_nrm 0x01 //Debug
exc_stub_nrm 0x02 //Non Maskable interrupt
exc_stub_nrm 0x03 //Breakpoexc
exc_stub_nrm 0x04 //Overflow
exc_stub_nrm 0x05 //Bound Range
exc_stub_nrm 0x06 //Invalid Opcode
exc_stub_nrm 0x07 //Device Not Aviable
exc_stub_err 0x08 //Double Fault
exc_stub_nrm 0x09 //Coprocessor Segment Overrun
exc_stub_err 0x0A //Invalid TTS
exc_stub_err 0x0B //Segment not Present
exc_stub_err 0x0C //Stack Fault
exc_stub_err 0x0D //General Protection
exc_stub_err 0x0E //Page Fault
exc_stub_nrm 0x0F //Reserved interrupt
exc_stub_nrm 0x10 //x87 Floating Poexc
exc_stub_err 0x11 //Alignment Check
exc_stub_nrm 0x12 //Machine Check
exc_stub_nrm 0x13 //SIMD Floating Poexc
exc_stub_nrm 0x14 //Reserved interrupt
exc_stub_nrm 0x15 //Reserved interrupt
exc_stub_nrm 0x16 //Reserved interrupt
exc_stub_nrm 0x17 //Reserved interrupt
exc_stub_nrm 0x18 //Reserved interrupt
exc_stub_nrm 0x19 //Reserved interrupt
exc_stub_nrm 0x1A //Reserved interrupt
exc_stub_nrm 0x1B //Reserved interrupt
exc_stub_nrm 0x1C //Reserved interrupt
exc_stub_nrm 0x1D //Reserved interrupt
exc_stub_nrm 0x1E //Reserved interrupt
exc_stub_nrm 0x1F //Reserved interrupt

.extern handle_exception
.extern common_eoi

common_exc_handler:
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
  call handle_exception
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
  
  add $8, %esp
  
  iret
