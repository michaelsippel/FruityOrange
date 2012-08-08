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
  set_interrupt(0x00, (uint32_t)&test_inthandler0x00,0x08,INTERRUPT_GATE, 0  );
  
  load_idt();
}

