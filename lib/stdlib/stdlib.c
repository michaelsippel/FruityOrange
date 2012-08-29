/**
 *  lib/stdlib/stdlib.c
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

long int labs(long int j){
  return (j < 0) ? (-j) : j;
}

int abs(int j){
  return (j < 0) ? (-j) : j;
}

div_t div(int count, int deniminator) {
  div_t data;
  data.quot = (int) (count / deniminator);
  data.rem = (int) (count % deniminator);
  
  return data;
}

ldiv_t ldiv(long int count, long int deniminator) {
  ldiv_t data;
  data.quot = (long int) (count / deniminator);
  data.rem = (long int) (count % deniminator);
  
  return data;
}
