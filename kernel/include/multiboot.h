/**
 *  kernel/include/multiboot.h
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
#ifndef _MULTIBOOT_H
#define _MULTIBOOT_H

#include <stdint.h>

extern const void kernel_start;
extern const void kernel_end;

#define KERNEL_SIZE (&kernel_end - &kernel_start)
#define KERNEL_PAGES (KERNEL_SIZE / PAGE_SIZE +1)

struct multiboot_info {
  uint32_t mbs_flags;
  
  uint32_t mbs_mem_lower;
  uint32_t mbs_mem_upper;
  
  uint32_t mbs_boot_device;
  uint32_t mbs_cmdline;
  
  uint32_t mbs_mods_count;
  uint32_t mbs_mods_addr;
  
  uint64_t mbs_syms_low;
  uint64_t mbs_syms_high;
  
  uint32_t mbs_mmap_length;
  uint32_t mbs_mmap_addr;
  
  uint32_t mbs_drives_length;
  uint32_t mbs_config_table;
  uint32_t mbs_boot_loader_name;
  uint32_t mbs_apm_table;
  
  uint32_t mbs_vbe_control_info;
  uint16_t mbs_vbe_mode_info;
  uint16_t mbs_vbe_mode;
  uint16_t mbs_vbe_interface_seg;
  uint16_t mbs_vbe_interface_off;
} __attribute__((packed));

struct multiboot_mmap {
  uint32_t size;
  uint64_t base;
  uint64_t length;
  uint32_t type;
} __attribute__((packed));

struct multiboot_module {
  uint32_t mod_start;
  uint32_t mod_end;
  uint32_t string;
  uint32_t reserved;
} __attribute__((packed));

#endif
