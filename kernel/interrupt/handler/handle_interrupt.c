/**
 *  kernel/interrupt/handler/handle_interrupt.c
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
#include <stdint.h>
#include <stddef.h>

#include <debug/panic.h>
#include <driver/console.h>
#include <cpu.h>

const char *exception_msg[] = {
  "Divide by Zero",
  "Debug",
  "Non Maskable Interrupt",
  "Breakpoint",
  "Overflow",
  "Bound Range",
  "Invalid Opcode",
  "Device Not Aviable",
  "Double Fault",
  "Coprocessor Segment Overrun",
  "Invalid TTS",
  "Segment not Present",
  "Stack Fault",
  "General Protection",
  "Page Fault",
  "Reserved Interrupt",
  "x87 Floating Point",
  "Alignment Check",
  "Machine Check",
  "SIMD Floating Point",
  "Reserved Interrupt",
  "Reserved Interrupt",
  "Reserved Interrupt",
  "Reserved Interrupt",
  "Reserved Interrupt",
  "Reserved Interrupt",
  "Reserved Interrupt",
  "Reserved Interrupt",
  "Reserved Interrupt",
  "Reserved Interrupt",
  "Reserved Interrupt",
  "Reserved Interrupt",
};

static void (*irq_handler[16])(void) = {
  NULL, NULL,
  NULL, NULL,
  NULL, NULL,
  NULL, NULL,
  NULL, NULL,
  NULL, NULL,
  NULL, NULL,
  NULL, NULL
};

int set_irq_handler(int irq, void (*handler)(void)) {
  if(irq_handler[irq] == NULL) {
    irq_handler[irq] = handler;
    return 0;
  }
  return 1;
}

static cpu_state_t *new_cpu;
cpu_state_t* handle_interrupt(cpu_state_t *cpu) {
  new_cpu = cpu;
  if(cpu->intr <= 0x1f) {
    setColor(0xf4);
    printf("Exception occured: #%s!\n",exception_msg[cpu->intr]);
    printf("CPU-Dump:\n"
	   "EAX = 0x%x  EBX = 0x%x  ECX = 0x%x  EDX = 0x%x\n"
	   "ESI = 0x%x  EDI = 0x%x  EBP = 0x%x  ESP = 0x%x\n"
	   "EIP = 0x%x   CS = 0x%x   SS = 0x%x\n"
	   " DS = 0x%x   ES = 0x%x   FS = 0x%x   GS = 0x%x\n"
	   "EFLAGS = 0x%x  INTR = 0x%4x  ERRORCODE = 0x%x\n"
	   "Kernel stopped.",
	   cpu->eax, cpu->ebx, cpu->ecx, cpu->edx,
	   cpu->esi, cpu->edi, cpu->ebp, cpu->esp,
	   cpu->eip, cpu->cs,  cpu->ss,
	   cpu->ds,  cpu->es,  cpu->fs,  cpu->gs,
	   cpu->eflags, cpu->intr, cpu->error_code);
	   
    while(1) {
      asm volatile("cli; hlt");
    }
  } else if(cpu->intr <= 0x2f) {
    if(irq_handler[cpu->intr - 0x20] != NULL) {
      irq_handler[cpu->intr - 0x20]();
    }
  }
  
  return new_cpu;
}

void set_cpu_state(cpu_state_t *cpu) {
  new_cpu = cpu;
}

cpu_state_t *get_cpu_state(void) {
  return new_cpu;
}
