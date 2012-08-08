//
//  kernel/interrupt/handler/stubs.S
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

.macro int_stub_nrm nr
.global int_handler\nr
int_handler\nr:
    push $0
    push $\nr
    jmp common_intr_handler
.endm

.macro int_stub_err nr
.global int_handler\nr
int_handler\nr:
    push $0
    push $\nr
    jmp common_intr_handler
.endm

int_stub_nrm 0x00 //Divide by Zero
int_stub_nrm 0x01 //Debug
int_stub_nrm 0x02 //Non Maskable Interrupt
int_stub_nrm 0x03 //Breakpoint
int_stub_nrm 0x04 //Overflow
int_stub_nrm 0x05 //Bound Range
int_stub_nrm 0x06 //Invalid Opcode
int_stub_nrm 0x07 //Device Not Aviable
int_stub_err 0x08 //Double Fault
int_stub_nrm 0x09 //Coprocessor Segment Overrun
int_stub_err 0x0A //Invalid TTS
int_stub_err 0x0B //Segment not Present
int_stub_err 0x0C //Stack Fault
int_stub_err 0x0D //General Protection
int_stub_err 0x0E //Page Fault
int_stub_nrm 0x0F //Reserved Interrupt
int_stub_nrm 0x10 //x87 Floating Point
int_stub_err 0x11 //Alignment Check
int_stub_nrm 0x12 //Machine Check
int_stub_nrm 0x13 //SIMD Floating Point
int_stub_nrm 0x14 //Reserved Interrupt
int_stub_nrm 0x15 //Reserved Interrupt
int_stub_nrm 0x16 //Reserved Interrupt
int_stub_nrm 0x17 //Reserved Interrupt
int_stub_nrm 0x18 //Reserved Interrupt
int_stub_nrm 0x19 //Reserved Interrupt
int_stub_nrm 0x1A //Reserved Interrupt
int_stub_nrm 0x1B //Reserved Interrupt
int_stub_nrm 0x1C //Reserved Interrupt
int_stub_nrm 0x1D //Reserved Interrupt
int_stub_nrm 0x1E //Reserved Interrupt
int_stub_nrm 0x1F //Reserved Interrupt

int_stub_nrm 0x20
int_stub_nrm 0x21
int_stub_nrm 0x22
int_stub_nrm 0x23
int_stub_nrm 0x24
int_stub_nrm 0x25
int_stub_nrm 0x26
int_stub_nrm 0x27
int_stub_nrm 0x28
int_stub_nrm 0x29
int_stub_nrm 0x2A
int_stub_nrm 0x2B
int_stub_nrm 0x2C
int_stub_nrm 0x2D
int_stub_nrm 0x2E
int_stub_nrm 0x2F

.extern handle_interrupt
.extern common_eoi
common_intr_handler:
  push %ebp
  push %edi
  push %esi
  push %edx
  push %ecx
  push %ebx
  push %eax
  
  push %esp
  call handle_interrupt
  add $4, %esp
  
  pop %eax
  pop %ebx
  pop %ecx
  pop %edx
  pop %esi
  pop %edi
  pop %ebp
  
  call common_eoi
  add $8, %esp
  iret
