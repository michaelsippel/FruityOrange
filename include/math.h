/**
 *  include/math.h
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
#ifndef _MATH_H
#define _MATH_H

#define PI 3.1415926535897932384626433832795
#define E  2.818281828
#define pi() PI
#define e() E

#define BCD_DECODE(x) ((0b1111 & x) + (10 * (((0b1111 << 4) & x) >> 4)))

double ceil(double x);
double exp(double x);
double fabs(double x);
double floor(double x);
double ldexp(double x, int exp);
double pow(double base, double exp);
double perc(double part, double all);

#endif
