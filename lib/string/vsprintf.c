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

void general_intformat(char *buf, long value, int base) {
    char t[65];
    char *tmp;
    char n;
    
    if (base > 16) {
        return;
    }

    tmp = t + 64;
    *tmp = '\0';
    do {
        n = (char) (value % base);
	if(n > 9) *--tmp = (char) n + ('A'-10);
	else      *--tmp = (char) n + '0';
        value /= base;
    } while (value);
    
    while((*buf++ = *tmp++));
    *buf = '\0';
}

void hex_str(char *buf, uint64_t value, int num_chr) {
//   char tmp;
//   int i;
//   for(i = num_chr-1; i >= 0; i--) {
//     tmp = (char) (value >> (i*num_chr)) & 0xf;
//     
//     if(tmp > 9) *buf++ = (char) tmp + ('A'-9);
//     else 	*buf++ = (char) tmp + '0';
//   }
//   *buf = '\0';
  general_intformat(buf, value, 16);
}

void dec_str(char *buf, int value, int num_chr) {
//   char *tmp = "";
  general_intformat(buf, value, 10);
  
}

void oct_str(char *buf, int value, int num_chr) {
  general_intformat(buf, value, 8);
}

int str_to_num(const char *buf) {
  int num = 0;
  while(*buf) {
    num *= 10;
    num += *buf++ - '0';
  }
  return num;
}

int vsprintf(char *buffer, const char *fmt, va_list args) {
  int num_chr = 0;
  int ret = (int) buffer;
  
  int n = 0;
  char *tmp = "";

  while(*fmt) {
    if(*fmt == '%') {
	tmp = "";
	n = 0;
	
	fmt++;
	if(*fmt >= '0' && *fmt <= '9') {
	  while(*fmt >= '0' && *fmt <= '9') {
	    *tmp++ = *fmt++;
	    n++;
	  }
	  *tmp = '\0';
	  tmp -= n;
	  num_chr = str_to_num(tmp);
	} else {
	  num_chr = 0;
	}
	switch(*fmt++) {
	  case '%':
	    *buffer++ = '%';
	  case 'X':
	  case 'x':
	    n = va_arg(args, unsigned long int);
	    hex_str(tmp, n, num_chr ? num_chr : 4);
	    while((*buffer++ = *tmp++));
	    break;
	  case 'D':
	  case 'd':
	    n = va_arg(args, int);
	    dec_str(tmp, n, num_chr);
	    while((*buffer++ = *tmp++));
	    break;
	  case 'O':
	  case 'o':
	    n = va_arg(args, int);
	    oct_str(tmp, n, num_chr);
	    while((*buffer++ = *tmp++));
	    break;
	  case 'C':
	  case 'c':
	    n = va_arg(args, int);
	    *buffer++ = (char) n;
	    break;
	  case 'S':
	  case 's':
	    tmp = va_arg(args, char*);
	    while((*buffer++ = *tmp++));
	    break;
	};
	break;
    } else { 
	*buffer++ = *fmt++;
    }
  }
  ret = (int) buffer - ret;
  
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

