/**
 *  kernel/driver/cmos/rtc.c
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
#include <sys/syscalls.h>
#include <stdint.h>
#include <math.h>

#include <driver/cmos.h>
#include <mm.h>
#include <interrupt.h>
#include <portio.h>

static cmos_data_t *cmos_data;
static cmos_time_t *cmos_time;

void init_rtc(void) {
  set_irq_handler(0x8, &rtc_irq_handler);
  setup_syscall(SYSCALL_TIME, "time", &time_syscall_wrapper);
  
  cmos_time = malloc(sizeof(cmos_time_t));
  cmos_data = get_cmos_data();
  
  cmos_write_byte(CMOS_REGISTER_A, (cmos_data->registers.register_a & 0xF0) | 0x0F);
  cmos_write_byte(CMOS_REGISTER_B, cmos_data->registers.register_b | 0x40);
  
  update_time();
}

void rtc_irq_handler(void) {
  cmos_data->registers.register_c = cmos_read_byte(CMOS_REGISTER_C);
  update_time();
}

cmos_time_t *get_cmos_time(void) {
  return cmos_time;
}

void update_time(void) {
  cmos_time->second =       BCD_DECODE(cmos_read_byte(0x00));
  cmos_time->alarm_sec =    BCD_DECODE(cmos_read_byte(0x01));
  cmos_time->minute =       BCD_DECODE(cmos_read_byte(0x02));
  cmos_time->alarm_min =    BCD_DECODE(cmos_read_byte(0x03));
  cmos_time->hour =         BCD_DECODE(cmos_read_byte(0x04));
  cmos_time->alarm_hour =   BCD_DECODE(cmos_read_byte(0x05));
  cmos_time->week_day =     BCD_DECODE(cmos_read_byte(0x06)) - 1;
  cmos_time->day_in_month = BCD_DECODE(cmos_read_byte(0x07));
  cmos_time->month =        BCD_DECODE(cmos_read_byte(0x08));
  cmos_time->year =         BCD_DECODE(cmos_read_byte(0x09));
  cmos_time->century =      BCD_DECODE(cmos_read_byte(0x32));
}
