/**
 *  kernel/driver/cmos/mktm.c
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
#include <time.h>
#include <driver/cmos.h>

tm_t mktm(cmos_time_t *cmos_time) {
  tm_t tm;
  
  tm.sec = cmos_time->second;
  tm.min = cmos_time->minute;
  tm.hour= cmos_time->hour;
  
  tm.mday = cmos_time->day_in_month;
  tm.wday = cmos_time->week_day;
  tm.mon = cmos_time->month;
  tm.year= cmos_time->year + 2000;
  
  return tm;
}
