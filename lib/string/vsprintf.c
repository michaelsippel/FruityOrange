/**
 *  lib/string/vsprintf.c
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
#include <stdarg.h>

#include <string.h>


void hex_str(char *buf, uint64_t value, int num_chr) {
  char tmp;
  int i;
  for(i = --num_chr; i >= 0; i--) {
    tmp = (value & (0xf << i)) >> (i*num_chr);
    
    if(tmp > 9) *buf++ = (char) tmp + 'A';
    else 	*buf++ = (char) tmp + '0';
  }
  *buf = '\0';
}

int vsprintf(char *buffer, const char *fmt, va_list args) {
  int num_chr = 0;
  int ret = 0;
  int n = 0;
  char *tmp = "\0";

  while(*fmt) {
    if(*fmt == '%') {
	fmt++;
	if(*fmt >= '0' && *fmt <= '9') {
	  
// 	  num_chr = str_to_num();
	} else {
	  num_chr = 0;
	}
	switch(*fmt++) {
	  case '%':
	    *buffer++ = '%';
	  case 'X':
	  case 'x':
	    if(! num_chr) num_chr = 4;
	    n = va_arg(args, unsigned long int);
	    hex_str(tmp, n, num_chr);
	    while(*tmp) {
	      *buffer++ = *tmp++;
	    }
	    break;
	};
	break;
    } else { 
	*buffer++ = *fmt++;
	ret++;
    }
  }
  *buffer = '\0';
  
  return ret;
}

int sprintf(char *buffer, const char *format, ...) {
  va_list args;
  int i;
  
  va_start(args, format);
  i = vsprintf(buffer, format, args);
  va_end(args);
  
  return i;
}

