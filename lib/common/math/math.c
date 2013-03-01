/**
 *  include/lib/math.c
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
#include <math.h>

double ceil(double x) {
  if((x - (int)x) > 0) {
    return x+1;
  } else {
    return x;
  }
}

double exp(double x) {
  return pow(E, x);
}

double fabs(double x) {
  if(x < 0) return (-x);
  return x;
}

double floor(double x) {
  return (double)((int) x);
}

double ldexp(double x, int exp) {
  return pow(x*2,exp);
}

double pow(double base, double exp) {
  double ret = base;
  int i;
  for(i = 0; i < exp; i++) ret *= ret;
  return ret;
}

double perc(double part, double all) {
  return ( (part / all) * (double)100.0 );
}
