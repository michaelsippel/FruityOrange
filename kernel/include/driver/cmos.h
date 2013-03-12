/**
 *  kernel/include/driver/cmos.h
 *
 *  (C) Copyright 2013 Michael Sippel
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
#ifndef _CMOS_H
#define _CMOS_H

#include <stdint.h>
#include <time.h>

#define CMOS_REGISTER_A 0x0A
#define CMOS_REGISTER_B 0x0B
#define CMOS_REGISTER_C 0x0C
#define CMOS_REGISTER_D 0x0D

struct cmos_register  {
  uint8_t register_a;
  uint8_t register_b;
  uint8_t register_c;
  uint8_t register_d;
} __attribute__((packed));
typedef struct cmos_register cmos_register_t;

struct cmos_hardware_data {
  uint8_t post_diagnostig_status_byte;
  uint8_t shutdown_status_byte;
  uint8_t floppy_disk_type;
//   uint8_t reserved0;
  uint8_t hd_type;
//   uint8_t reserved1;
  uint8_t device_byte;
  
  uint8_t basememory_size_low;
  uint8_t basememory_size_high;
  uint8_t expandablememory_size_low;
  uint8_t expandablememory_size_high;
  uint8_t extension_byte_hd1;
  uint8_t extension_byte_hd2;

//   uint16_t reserved2;
//   uint8_t reserved3 : 2;
  
  uint8_t cmos_magic_low;
  uint8_t cmos_magic_high;
  
  uint8_t extendedmemory_low;
  uint8_t extendedmenory_high;
  
//   uint16_t reserved4 : 13;
} __attribute__((packed));
typedef struct cmos_hardware_data cmos_hardware_data_t;

struct cmos_time {
  uint8_t second;
  uint8_t alarm_sec;
  uint8_t minute;
  uint8_t alarm_min;
  uint8_t hour;
  uint8_t alarm_hour;
  uint8_t week_day;
  uint8_t day_in_month;
  uint8_t month;
  uint8_t year;
  uint8_t century;
} __attribute__((packed));
typedef struct cmos_time cmos_time_t;

struct cmos_data {
  cmos_time_t *time;
  cmos_register_t registers;
  cmos_hardware_data_t hardware;
} __attribute__((packed));
typedef struct cmos_data cmos_data_t;

void init_cmos(void);
void init_rtc(void);
cmos_data_t *get_cmos_data(void);
cmos_time_t *get_cmos_time(void);

void rtc_irq_handler(void);
void update_time(void);

uint8_t cmos_read_byte(uint8_t offset);
void cmos_write_byte(uint8_t offset, uint8_t value);

tm_t mktm(cmos_time_t *cmos_time);
void time_syscall_wrapper(uint32_t *ebx, uint32_t *ecx, uint32_t *edx);

#endif
