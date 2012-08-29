/**
 *  lib/stdlib/atoX.c
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
#include <stdlib.h>

int atoi(const char *string) {
  int num = 0;
  while(*string) {
    num *= 10;
    num += *string++ - '0';
  }
  return num;
}

long int atol(const char *string) {
  long int num = 0;
  while(*string) {
    num *= 10;
    num += *string++ - '0';
  }
  return num;
}
