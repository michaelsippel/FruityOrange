/**
 *  kernel/gdt.c
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
#define _GDT_C
#include <stdint.h>
#include <init/gdt.h>

//Acessbyte
#define GDT_PRESENT 0x80
#define GDT_SEGMENT 0x10
#define GDT_RING0   0x00
#define GDT_RING3   0x60
#define GDT_CODESEG_NR 0x00
#define GDT_CODESEG_RD 0x0A
#define GDT_DATASEG_RD 0x00
#define GDT_DATASEG_WR 0x02
#define GDT_TSS        0x09

//Flags
#define GDT_4K_GRAN 0x800
#define GDT_32_BIT  0x400

typedef struct {
  uint16_t limit_low;
  uint16_t base_low;
  uint8_t base_middle;
  uint8_t access;
  uint8_t limit_high :4;
  uint8_t flags :4;
  uint8_t base_high;
} GDT_ENTRY;

typedef union {
  GDT_ENTRY struct_type;
  uint64_t entry;
} GDT_CONVERTER;


uint64_t gdt[GDT_ENTRIES];
uint32_t tss[TSS_SIZE] = {0, 0, 0x10};

uint64_t create_gdt_entry(uint32_t limit, uint32_t base, uint16_t flags) {
    GDT_ENTRY ent;
    GDT_CONVERTER conv;
    
    ent.limit_low = limit&0x0ffff;
    ent.base_low  = base&0x0000ffff;
    ent.base_middle = (base&0x00ff0000)>>16;
    ent.access      = flags&0x00ff;
    ent.limit_high  = (limit&0xf0000)>>16;
    ent.flags       = (flags&0x0f00)>>8;
    ent.base_high   = (base&0xff000000)>>24;
    
    conv.struct_type = ent;
    return conv.entry;
}

void load_gdt(void) {
  struct {
    uint16_t size;
    uint64_t *pointer;
  } __attribute__((packed)) gdtp = {
    .size = GDT_ENTRIES * 8 - 1,
    .pointer = gdt,
  };
  asm volatile("lgdt %0" : : "m" (gdtp));
}

void init_gdt(void) {
  //Null
  gdt[GDT_ENTRY_NULL] = 0;
  
  //Kernel -code
  gdt[GDT_ENTRY_KERNEL_CODE] = create_gdt_entry(0xfffff, 0,
      GDT_CODESEG_RD | GDT_SEGMENT | GDT_RING0 | GDT_PRESENT |
      GDT_32_BIT  | GDT_4K_GRAN);	
  //Kernel -data
  gdt[GDT_ENTRY_KERNEL_DATA] = create_gdt_entry(0xfffff, 0,
      GDT_DATASEG_WR | GDT_SEGMENT | GDT_RING0 | GDT_PRESENT |
      GDT_32_BIT  | GDT_4K_GRAN);
  
  //Userspace -code
  gdt[GDT_ENTRY_USER_CODE] = create_gdt_entry(0xfffff, 0,
      GDT_CODESEG_RD | GDT_SEGMENT | GDT_RING3 | GDT_PRESENT |
      GDT_32_BIT  | GDT_4K_GRAN);
  //Userspace -data 
  gdt[GDT_ENTRY_USER_DATA] = create_gdt_entry(0xfffff, 0,
      GDT_DATASEG_WR | GDT_SEGMENT | GDT_RING3 | GDT_PRESENT |
      GDT_32_BIT  | GDT_4K_GRAN);
      
  //TSS
  gdt[GDT_ENTRY_TSS] = create_gdt_entry(sizeof(tss), (uint32_t) tss,
      GDT_TSS | GDT_RING3 | GDT_PRESENT);
  
  load_gdt();
  // load task register
  asm volatile("ltr %%ax" : : "a" (GDT_ENTRY_TSS << 3));
  // reload segment registers
  asm volatile(
	"ljmpl $0x08, $1f\n\t"
	"1:\n\t"
        "mov $0x10, %eax\n\t"
        "mov %eax, %ds\n\t"
        "mov %eax, %es\n\t"
        "mov %eax, %fs\n\t"
        "mov %eax, %gs\n\t"
        "mov %eax, %ss\n\t"
  );
}

