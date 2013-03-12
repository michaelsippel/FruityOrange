/**
 *  lib/common/string/vsprintf.c
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
#include <stdlib.h>

#include <string.h>

void general_uintformat(char *buf, unsigned long value, int base, int num_chr) {
    char n;
    char *tmp;
    char t[65];
    
    int i = 0;
    int min_num_chr = 0;
    long dummy = value;
    while((dummy /= base)) min_num_chr++;
    if(min_num_chr > num_chr) num_chr = min_num_chr;
    
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
	i++;
    } while (value);
    
    while(i++ < num_chr){
	*--tmp = '0';
    }
    
    while((*buf++ = *tmp++));
}

void hex_str(char *buf, unsigned long value, int num_chr) {
  general_uintformat(buf, value, 16, (num_chr-1) ? num_chr : 8);
}

void int_dec_str(char *buf, long value, int num_chr, int signd) {
  if( (value & 0x80000000) && signd) {
    general_uintformat(buf+1, (-value), 10, num_chr);
    *buf = '-';
  } else {
    general_uintformat(buf, value, 10, num_chr);
  }
}

void oct_str(char *buf, unsigned long value, int num_chr) {
  general_uintformat(buf, value, 8, (num_chr-1) ? num_chr : 4);
}

int vsprintf(char *buffer, const char *fmt, va_list args) {
  #define COPY while(*tmp) *buffer++ = *tmp++;
  int num_chr = 1;
  int ret = (int) buffer;
  
  long n = 0;
  char *tmp = "";
  
  while(*fmt) {
    if(*fmt == '%') {
	fmt++;
	tmp = "";
	n = 0;
	
	#define IS_FMT_NUM (*fmt >= '0' && *fmt <= '9')
	if(IS_FMT_NUM) {
	  while(IS_FMT_NUM) {
	    *tmp++ = *fmt++;
	    n++;
	  }
	  *tmp = '\0';
	  tmp -= n;
	  num_chr = atoi(tmp+1);
	} else {
	  num_chr = 1;
	}
	switch(*fmt++) {
	  case '%':
	    *buffer++ = '%';
	  case 'X':
	  case 'x':
	    n = va_arg(args, unsigned long int);
	    hex_str(tmp, n, num_chr);
	    COPY;
	    break;
	  case 'D':
	  case 'd':
	    n = va_arg(args, int);
	    int_dec_str(tmp, n, num_chr, 1);
	    COPY;
	    break;
	  case 'U':
	  case 'u':
	    n = va_arg(args, unsigned int);
	    int_dec_str(tmp, n, num_chr, 0);
	    COPY;
	    break;
	  case 'L':
	  case 'l':
	    switch(*fmt++) {
	      case 'D':
	      case 'd':
		n = va_arg(args, long int);
		int_dec_str(tmp, n, num_chr, 1);
		COPY;
		break;
	      case 'U':
	      case 'u':
		n = va_arg(args, unsigned long int);
		int_dec_str(tmp, n, num_chr, 0);
		COPY;
		break;
	    }
	    break;
	  case 'O':
	  case 'o':
	    n = va_arg(args, unsigned long int);
	    oct_str(tmp, n, num_chr);
	    COPY;
	    break;
	  case 'C':
	  case 'c':
	    n = va_arg(args, int);
	    *buffer++ = (char) n;
	    break;
	  case 'S':
	  case 's':
	    tmp = va_arg(args, char*);
	    COPY;
	    break;
	}
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

