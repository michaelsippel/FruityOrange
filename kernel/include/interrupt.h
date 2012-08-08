/**
 *  kernel/include/interrupt.h
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
#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#include <stdint.h>

#define INTERRUPT_GATE	0x06
#define TRAP_GATE	0x07
#define TASK_GATE	0x05

#define FIRST_EXEPTION 0x00
#define EXEPTION_NUM   0x1F
#define LAST_EXEPTION  (FIRST_EXEPTION + EXEPTION_NUM)

#define IRQ_NUM   0xF
#define FIRST_IRQ 0x20
#define LAST_IRQ  (FIRST_IRQ + IRQ_NUM)

void init_idt(void);
void load_idt(void);

void init_pic(void);
void send_eoi(uint8_t irq);
void common_eoi(uint32_t intrpt);

#define sti() asm volatile("sti");
#define cli() asm volatile("cli");

#endif
