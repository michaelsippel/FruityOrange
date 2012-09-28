/**
 *  kernel/include/gdt.h
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
#ifndef _GDT_H
#define _GDT_H

#include <stdint.h>

#define GDT_ENTRY_NULL 0
#define GDT_ENTRY_KERNEL_CODE 1
#define GDT_ENTRY_KERNEL_DATA 2
#define GDT_ENTRY_USER_CODE 3
#define GDT_ENTRY_USER_DATA 4
#define GDT_ENTRY_TSS 5

#define GDT_ENTRIES 6
#define TSS_SIZE 32

void init_gdt(void);

void load_gdt(void);
uint64_t create_gdt_entry(uint32_t limit, uint32_t base, uint16_t flags);

#endif
