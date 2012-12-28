/**
 *  kernel/interrupt/handler/irq.c
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

#include <interrupt.h>
#include <debug/panic.h>
#include <driver/console.h>
#include <cpu.h>

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

cpu_state_t* handle_irq(cpu_state_t *cpu) {
  new_cpu = cpu;
  if(irq_handler[cpu->intr - 0x20] != NULL) {
    irq_handler[cpu->intr - 0x20]();
  }
  
  return new_cpu;
}
