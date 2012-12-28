/**
 *  kernel/interrupt/idt.h
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
#include <stdbool.h>

#define _NEW_CPU_STRUCT
#include <interrupt.h>
#include "handler.h"

#define IDT_ENTRIES 0xFF

typedef struct {
  uint16_t offset_low;
  uint16_t selector;
  uint8_t ist :3;
  uint8_t reserved :5;
  uint8_t type :3;
  uint8_t d :2;
  uint8_t dpl :2;
  uint8_t present :1;
  uint16_t offset_high;
} IDT_ENTRY;

typedef union {
  IDT_ENTRY struct_type;
  uint64_t entry;
} IDT_CONVERTER;


static uint64_t idt[IDT_ENTRIES];
cpu_state_t *new_cpu;

static void set_interrupt(int i, uint32_t offset, uint16_t selector, uint8_t type, int dpl) {
  IDT_ENTRY ent;
  IDT_CONVERTER conv;
  
  ent.offset_low = offset&0x0000ffff;
  ent.selector   = selector;
  ent.ist = 0;
  ent.reserved = 0;
  ent.type = type;
  ent.d = 1;
  ent.dpl = dpl&3;
  ent.present = 1;
  ent.offset_high = (offset&0xffff0000)>>16;
  
  conv.struct_type = ent;
  idt[i] = conv.entry;
}

void load_idt(void) {
  struct {
    uint16_t size;
    uint64_t *pointer;
  } __attribute__((packed)) idtp = {
    .size = IDT_ENTRIES * 8 - 1,
    .pointer = idt,
  };
  asm volatile("lidt %0" : : "m" (idtp));
}

void init_idt(void) {
  ///           index, offset,                  selector,  type,     dpl
  set_interrupt(0x00, (uint32_t)&int_handler0x00,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x01, (uint32_t)&int_handler0x01,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x02, (uint32_t)&int_handler0x02,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x03, (uint32_t)&int_handler0x03,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x04, (uint32_t)&int_handler0x04,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x05, (uint32_t)&int_handler0x05,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x06, (uint32_t)&int_handler0x06,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x07, (uint32_t)&int_handler0x07,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x08, (uint32_t)&int_handler0x08,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x09, (uint32_t)&int_handler0x09,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x0A, (uint32_t)&int_handler0x0A,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x0B, (uint32_t)&int_handler0x0B,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x0C, (uint32_t)&int_handler0x0C,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x0D, (uint32_t)&int_handler0x0D,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x0E, (uint32_t)&int_handler0x0E,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x0F, (uint32_t)&int_handler0x0F,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x10, (uint32_t)&int_handler0x10,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x11, (uint32_t)&int_handler0x11,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x12, (uint32_t)&int_handler0x12,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x13, (uint32_t)&int_handler0x13,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x14, (uint32_t)&int_handler0x14,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x15, (uint32_t)&int_handler0x15,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x16, (uint32_t)&int_handler0x16,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x17, (uint32_t)&int_handler0x17,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x18, (uint32_t)&int_handler0x18,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x19, (uint32_t)&int_handler0x19,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x1A, (uint32_t)&int_handler0x1A,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x1B, (uint32_t)&int_handler0x1B,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x1C, (uint32_t)&int_handler0x1C,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x1D, (uint32_t)&int_handler0x1D,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x1E, (uint32_t)&int_handler0x1E,0x08,INTERRUPT_GATE, 0  );//Exeption
  set_interrupt(0x1F, (uint32_t)&int_handler0x1F,0x08,INTERRUPT_GATE, 0  );//Exeption
  
  set_interrupt(0x20, (uint32_t)&int_handler0x20,0x08,INTERRUPT_GATE, 0  );//IRQ
  set_interrupt(0x21, (uint32_t)&int_handler0x21,0x08,INTERRUPT_GATE, 0  );//IRQ
  set_interrupt(0x22, (uint32_t)&int_handler0x22,0x08,INTERRUPT_GATE, 0  );//IRQ
  set_interrupt(0x23, (uint32_t)&int_handler0x23,0x08,INTERRUPT_GATE, 0  );//IRQ
  set_interrupt(0x24, (uint32_t)&int_handler0x24,0x08,INTERRUPT_GATE, 0  );//IRQ
  set_interrupt(0x25, (uint32_t)&int_handler0x25,0x08,INTERRUPT_GATE, 0  );//IRQ
  set_interrupt(0x26, (uint32_t)&int_handler0x26,0x08,INTERRUPT_GATE, 0  );//IRQ
  set_interrupt(0x27, (uint32_t)&int_handler0x27,0x08,INTERRUPT_GATE, 0  );//IRQ
  set_interrupt(0x28, (uint32_t)&int_handler0x28,0x08,INTERRUPT_GATE, 0  );//IRQ
  set_interrupt(0x29, (uint32_t)&int_handler0x29,0x08,INTERRUPT_GATE, 0  );//IRQ
  set_interrupt(0x2A, (uint32_t)&int_handler0x2A,0x08,INTERRUPT_GATE, 0  );//IRQ
  set_interrupt(0x2B, (uint32_t)&int_handler0x2B,0x08,INTERRUPT_GATE, 0  );//IRQ
  set_interrupt(0x2C, (uint32_t)&int_handler0x2C,0x08,INTERRUPT_GATE, 0  );//IRQ
  set_interrupt(0x2D, (uint32_t)&int_handler0x2D,0x08,INTERRUPT_GATE, 0  );//IRQ
  set_interrupt(0x2E, (uint32_t)&int_handler0x2E,0x08,INTERRUPT_GATE, 0  );//IRQ
  set_interrupt(0x2F, (uint32_t)&int_handler0x2F,0x08,INTERRUPT_GATE, 0  );//IRQ
  
  set_interrupt(0x30, (uint32_t)&int_handler0x30,0x08,INTERRUPT_GATE, 0  );//Syscall
  
  load_idt();
}

void set_cpu_state(cpu_state_t *cpu) {
  new_cpu = cpu;
}

cpu_state_t *get_cpu_state(void) {
  return new_cpu;
}
