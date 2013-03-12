/**
 *  lib/common/time.c
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

time_t mktime(tm_t tm) {
  time_t time;
  uint16_t year = tm.year - TIME_EPOCH_START;
  
  time = 
    YEARS_TO_SEC(year) + 
    MON_TO_SEC(tm.mon) +
    DAYS_TO_SEC(tm.day) +
    HOURS_TO_SEC(tm.hour) +
    MIN_TO_SEC(tm.min) +
    tm.sec;
  
  return time;
}

tm_t rdtime(time_t time) {
  tm_t tm;
  
  tm.year = (uint16_t) SEC_TO_YEARS(time) + TIME_EPOCH_START;
  time -= (time_t) YEARS_TO_SEC(tm.year);
  
  tm.mon = (uint8_t) SEC_TO_MON(time);
  time -= (time_t) MON_TO_SEC(tm.mon);
  
  tm.day = (uint8_t) SEC_TO_DAYS(time);
  time -= (time_t) DAYS_TO_SEC(tm.day);
  
  
  tm.hour = (uint8_t) SEC_TO_HOURS(time);
  time -= (time_t) HOURS_TO_SEC(tm.hour);
  
  tm.min = (uint8_t) SEC_TO_MIN(time);
  time -= (time_t) MIN_TO_SEC(tm.min);
  
  tm.sec = (uint8_t) time;
  
  return tm;
}
