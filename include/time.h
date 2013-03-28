/**
 *  include/time.h
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
#ifndef _TIME_H
#define _TIME_H

#include <stdint.h>

#define TIME_EPOCH_START 2000

#define NUM_DAYS_IN_YEAR 365
#define NUM_DAYS_IN_WEEK   7
#define NUM_MON_IN_YEAR   12
#define NUM_HOURS_IN_DAY  24
#define NUM_MIN_IN_HOUR   60
#define NUM_SEC_IN_MIN   ( 60 )
#define NUM_SEC_IN_HOUR  ( 60 * 60 )
#define NUM_SEC_IN_DAY   ( 60 * 60 * 24 )
#define NUM_SEC_IN_YEAR  ( 60 * 60 * 24 * 365 )

#define LEAP_DAYS(x) ( (x / 4) - (x / 100) + (x / 400) )
#define LEAP_YEAR(x) ( (!(x % 4) && (x % 100)) || !(x % 400) )
#define YEAR_SIZE(x) ( LEAP_YEAR(x) ? 366 : 365 )

typedef unsigned long time_t;

typedef struct tm {
  uint8_t sec;
  uint8_t min;
  uint8_t hour;
  
  uint8_t mon;
  uint8_t mday;
  uint8_t wday;
  uint8_t yday;
  uint16_t year;
} tm_t;

time_t mktime(tm_t tm);
tm_t gmtime(time_t time);

#ifndef _NO_USER_INC
#include "user/time.h"
#endif

#endif
