/**
 *  include/stdlib.h
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
#ifndef _STDLIB_H
#define _STDLIB_H

#include <stdint.h>

typedef struct div {
  int quot;
  int rem;
} div_t;

typedef struct ldiv {
  long int quot;
  long int rem;
} ldiv_t;

long int labs(long int j);
int abs(int j);
int atoi(const char *string);
long int atol(const char *string);
div_t div(int count, int deniminator);
ldiv_t ldiv(long int count, long int deniminator);

#endif
