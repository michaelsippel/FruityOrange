/**
 *  include/user/time.h
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
#ifndef _USER_TIME_H
#define _USER_TIME_H

#include <time.h>

struct timeval {
    time_t sec;
    time_t usec;
} timeval_t;

struct timezone {
    int minuteswest;
    int dsttime;
} timezone_t;

int gettimeofday(struct timeval *tv, struct timezone *tz);
time_t time(void);

#endif
