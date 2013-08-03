/**
 *  kernel/interrupt/pic.c
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

#include <portio.h>
#include <interrupt.h>

#define PIC_MASTER_COMM    0x20
#define PIC_MASTER_DATA    (PIC_MASTER_COMM) + 1
#define PIC_SLAVE_COMM     0xa0
#define PIC_SLAVE_DATA     (PIC_SLAVE_COMM) + 1

#define PIC_INIT        0x11
#define PIC_ICW4        0x01
#define PIC_EOI         0x20


void init_pic(void) {
  outb(PIC_MASTER_COMM,PIC_INIT);
  outb(PIC_SLAVE_COMM,PIC_INIT);
  
  outb(PIC_MASTER_DATA, FIRST_IRQ);
  outb(PIC_SLAVE_DATA,  FIRST_IRQ + 8);
  
  outb(PIC_MASTER_DATA, 0x04);
  outb(PIC_SLAVE_DATA,  0x02);
  
  outb(PIC_MASTER_DATA, PIC_ICW4);
  outb(PIC_SLAVE_DATA,  PIC_ICW4);
  
  outb(PIC_MASTER_DATA, 0x0);
  outb(PIC_SLAVE_DATA,  0x0);
}

void send_eoi(uint8_t irq) {
  if (irq >= 8) {
    outb(PIC_SLAVE_COMM, PIC_EOI);
  }
  outb(PIC_MASTER_COMM, PIC_EOI);
}

void common_eoi(uint32_t intrpt) {
  if(intrpt >= 0x20 && intrpt <= 0x2f) {
    if (intrpt >= 0x28) {
      outb(PIC_SLAVE_COMM, PIC_EOI);
    }
    outb(PIC_MASTER_COMM, PIC_EOI);
  }
}

