/**
 *  include/time.h
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
#ifndef _TIME_H
#define _TIME_H

#include <stdint.h>

#define TIME_EPOCH_START 2000

#define NUM_DAYS_IN_YEAR 365
#define NUM_MON_IN_YEAR   12
#define NUM_DAYS_IN_MON   30
#define NUM_HOURS_IN_DAY  24
#define NUM_MIN_IN_HOUR   60
#define NUM_SEC_IN_MIN    60

#define YEARS_TO_MON(x)  (x * NUM_MON_IN_YEAR)
#define YEARS_TO_DAYS(x) (x * NUM_DAYS_IN_YEAR)
#define MON_TO_DAYS(x)   (x * NUM_DAYS_IN_MON)
#define DAYS_TO_HOURS(x) (x * NUM_HOURS_IN_DAY)
#define HOURS_TO_MIN(x)  (x * NUM_MIN_IN_HOUR)
#define MIN_TO_SEC(x)    (x * NUM_SEC_IN_MIN)

#define HOURS_TO_SEC(x) ( MIN_TO_SEC(HOURS_TO_MIN(x))    )
#define DAYS_TO_SEC(x)  ( HOURS_TO_SEC(DAYS_TO_HOURS(x)) )
#define MON_TO_SEC(x)   ( DAYS_TO_SEC(MON_TO_DAYS(x))    )
#define YEARS_TO_SEC(x) ( DAYS_TO_SEC(YEARS_TO_DAYS(x))  )


#define SEC_TO_MIN(x)    (x / NUM_SEC_IN_MIN)
#define MIN_TO_HOURS(x)  (x / NUM_MIN_IN_HOUR)
#define HOURS_TO_DAYS(x) (x / NUM_HOURS_IN_DAY)
#define DAYS_TO_MON(x)   (x / NUM_DAYS_IN_MON)
#define DAYS_TO_YEARS(x) (x / NUM_DAYS_IN_YEAR)
#define MON_TO_YEARS(x)  (x / NUM_MON_IN_YEAR)

#define SEC_TO_HOURS(x) ( MIN_TO_HOURS(SEC_TO_MIN(x))     )
#define SEC_TO_DAYS(x)  ( HOURS_TO_DAYS(SEC_TO_HOURS(x))  )
#define SEC_TO_MON(x)   ( DAYS_TO_MON(SEC_TO_DAYS(x))     )
#define SEC_TO_YEARS(x) ( DAYS_TO_YEARS(SEC_TO_DAYS(x))   )

typedef unsigned long time_t;

typedef struct tm {
  uint8_t sec;
  uint8_t min;
  uint8_t hour;
  
  uint8_t day;
  uint8_t mon;
  uint16_t year;
} tm_t;

time_t mktime(tm_t tm);
tm_t rdtime(time_t time);

#ifndef _NO_USER_INC
#include "user/time.h"
#endif

#endif
