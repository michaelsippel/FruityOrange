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

static uint16_t begin_days[12] = {0,31,59,90,120,151,181,212,243,273,304,334};
static uint16_t begin_days_leap_year[12] = {0,31,60,91,121,152,182,213,244,274,305,335};

time_t mktime(tm_t tm) {
  time_t time = 0;
  uint16_t years = tm.year - TIME_EPOCH_START;
  uint16_t leaps = LEAP_DAYS(years);
  
  time += tm.sec;
  time += tm.min  * NUM_SEC_IN_MIN;
  time += tm.hour * NUM_SEC_IN_HOUR;
  time += ( begin_days[tm.mon-1] + tm.mday-1 ) * NUM_SEC_IN_DAY;
  time += years * NUM_SEC_IN_YEAR;
  time += leaps * NUM_SEC_IN_DAY;
  
  return time;
}

tm_t gmtime(time_t time) {
  tm_t tm;
  
  long year = time / NUM_SEC_IN_YEAR;
  
  long dayclock = time % NUM_SEC_IN_DAY;
  long day      = time / NUM_SEC_IN_DAY;
  
  tm.sec = dayclock % NUM_SEC_IN_MIN;
  tm.min = (dayclock % NUM_SEC_IN_HOUR) / NUM_SEC_IN_MIN;
  tm.hour = dayclock / NUM_SEC_IN_HOUR;
  
  tm.year = (uint16_t) year + TIME_EPOCH_START;
  
  while (day >= YEAR_SIZE(year)) {
    day -= YEAR_SIZE(year);
    year++;
  }
  
  tm.yday = (uint16_t) day +1;
  
  if(LEAP_YEAR(year)) {
    for(tm.mon = 0; tm.yday > begin_days_leap_year[tm.mon]; tm.mon++);
  } else {
    for(tm.mon = 0; tm.yday > begin_days[tm.mon]; tm.mon++);
  }
  
  tm.mday = (uint8_t) tm.yday - begin_days[tm.mon-1];
  tm.wday = (uint8_t) ( (day + 6) % NUM_DAYS_IN_WEEK);
  
  return tm;
}

