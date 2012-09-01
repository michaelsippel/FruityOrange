/**
 *  kernel/console.c
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

#include <portio.h>
#include <driver/console.h>

#define TABULATOR_SIZE 8

static int videotext_width  = 80;
static int videotext_height = 25;

static uint16_t *video_mem = (uint16_t*) 0xB8000;
static uint8_t color = 0x07;
static int x = 0, y = 0;

#define PUTC(c) video_mem[(x++) + (y*80)] = ( c | (color << 8) );
#define NEWLINE y ++; x = 0;

void videotext_control(void) {
  if(x > videotext_width) {
    NEWLINE;
  } else if(x < 0) {
    x = videotext_width-1;
    y --;
  }
  
  if(y == videotext_height-1 && x == videotext_width) {
    scroll();
    NEWLINE;
  }
  if(y >= videotext_height) {
    scroll();
  }
}

int putchar(char chr) {
  PUTC(chr);
  videotext_control();
  setCursor(x, y);
  return 0;
}

int puts(const char *str) {
  int i = 0;

  while(*str) {
    switch(*str){
      case '\n':
	NEWLINE;
	break;
      case '\t':
	while(++x % TABULATOR_SIZE != 0);
	break;
      case '\r':
	x--;
	PUTC(' ');
	x--;
	break;
      default:
	PUTC(*str);
	break;
    }
    videotext_control();
    i++;
    str++;
  }
  setCursor(x, y);
  return i;
}

int printf(const char *format, ...) {
  va_list args;
  int i = 1, j;
  
//   char *buffer = format;
//   while(*buffer++) i++;
  char buffer[1024];
  
  va_start(args, format);
  i = vsprintf(buffer, format, args);
  va_end(args);
  
  j = puts(buffer);
  
  if(j == i)
    return i;
  else
    return -1;
}

void scroll(void) {
  y --;
  setCursor(x, y);
  int i;
  for(i = 0; i < (videotext_width * (videotext_height-1)); i++) {
    video_mem[i] = video_mem[i + videotext_width];
  }
  for(i = 0; i < (videotext_width * (videotext_height-1)); i++) {
    video_mem[i + (videotext_width * (videotext_height-1))] = (color << 8);
  }
}

void setCursor(int cx, int cy) {
  uint16_t tmp;
  tmp = cy * videotext_width + cx;
  outb(0x3D4,14);
  outb(0x3D5,tmp >> 8);
  outb(0x3D4,15);
  outb(0x3D5,tmp);
}

void setColor(uint8_t ncolor) {
  color = ncolor;
}

void setForegroundColor(uint8_t fcolor) {
  color = ( (color&0xf0) | fcolor );
}

void setBackgroundcolor(uint8_t bcolor) {
  color = ( (color&0x0f) | (bcolor << 4) );
}

void clearscreen(void) {
  int i;
  for( i = 0; i < (videotext_width * videotext_height); i++){
    video_mem[i] = color << 8;
  }
  setCursor(0, 0);
}
