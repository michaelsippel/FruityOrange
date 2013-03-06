/**
 *  kernel/driver/cmos/cmos.c
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

#include <driver/cmos.h>
#include <mm.h>
#include <portio.h>

static cmos_data_t *cmos_data;

void init_cmos(void) {
  cmos_data = malloc(sizeof(cmos_data_t));
  
  cmos_data->registers.register_a = cmos_read_byte(CMOS_REGISTER_A);
  cmos_data->registers.register_b = cmos_read_byte(CMOS_REGISTER_B);
  cmos_data->registers.register_c = cmos_read_byte(CMOS_REGISTER_C);
  cmos_data->registers.register_d = cmos_read_byte(CMOS_REGISTER_D);
  
  init_rtc();
  
  cmos_data->hardware.post_diagnostig_status_byte = cmos_read_byte(0x0E);
  cmos_data->hardware.shutdown_status_byte =        cmos_read_byte(0x0F);
  cmos_data->hardware.floppy_disk_type =            cmos_read_byte(0x10);
  cmos_data->hardware.hd_type =                     cmos_read_byte(0x12);
  cmos_data->hardware.device_byte =                 cmos_read_byte(0x14);
  
  cmos_data->hardware.basememory_size_low =         cmos_read_byte(0x15);
  cmos_data->hardware.basememory_size_high =        cmos_read_byte(0x16);
  cmos_data->hardware.expandablememory_size_low =   cmos_read_byte(0x17);
  cmos_data->hardware.expandablememory_size_high =  cmos_read_byte(0x18);
  cmos_data->hardware.extension_byte_hd1 =          cmos_read_byte(0x19);
  cmos_data->hardware.extension_byte_hd2 =          cmos_read_byte(0x1A);
  
  cmos_data->hardware.cmos_magic_low =              cmos_read_byte(0x2E);
  cmos_data->hardware.cmos_magic_high =             cmos_read_byte(0x2F);
  
  cmos_data->hardware.extendedmemory_low =          cmos_read_byte(0x30);
  cmos_data->hardware.extendedmenory_high =         cmos_read_byte(0x31);
}

cmos_data_t *get_cmos_data(void) {
  return cmos_data;
}

uint8_t cmos_read_byte(uint8_t offset) {
  uint8_t tmp = inb(0x70);
  outb(0x70, (tmp & 0x80) | (offset & 0x7F));
  
  return inb(0x71);
}

void cmos_write_byte(uint8_t offset, uint8_t value) {
  uint8_t tmp = inb(0x70);
  outb(0x70, (tmp & 0x80) | (offset & 0x7F));
  outb(0x71,value);
}
