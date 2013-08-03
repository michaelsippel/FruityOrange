//
//  kernel/interrupt/handler/irq.S
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
.extern handle_irq

.macro irq_stub nr
.global int_handler\nr
int_handler\nr:
    push $0
    push $\nr
    jmp common_irq_handler
.endm

irq_stub 0x20 // PIT
irq_stub 0x21 // Keyboard (KBC)
irq_stub 0x22 // twice PIC
irq_stub 0x23 // RS-232
irq_stub 0x24 // RS-232
irq_stub 0x25 // LPT 2
irq_stub 0x26 // FDC
irq_stub 0x27 // LPT 1 & Spurious interrupt
irq_stub 0x28 // RTC
irq_stub 0x29 // free
irq_stub 0x2A // 4. ATA / ATAPI / (E)IDE
irq_stub 0x2B // 3. ATA / ATAPI / (E)IDE
irq_stub 0x2C // Mouse (KBC)
irq_stub 0x2D // FPU
irq_stub 0x2E // 1. ATA / ATAPI / (E)IDE
irq_stub 0x2F // 2. ATA / ATAPI / (E)IDE

.extern handle_irq
.extern common_eoi

common_irq_handler:
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
  call handle_irq
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
