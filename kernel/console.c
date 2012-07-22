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
#include <portio.h>

#include <console.h>

#define VIDEOTEXT_WIDTH  80
#define VIDEOTEXT_HEIGHT 25

#define TABULATOR_SIZE 8

static uint16_t *video_mem = (uint16_t*) 0xB8000;
static uint8_t color = 0x07;
static int x = 0, y = 0;

#define VIDEOTEXT_CONTROL if(x > VIDEOTEXT_WIDTH) { \
			    x = 1; \
			    if(++y > VIDEOTEXT_HEIGHT){ \
			      scroll(); \
			    } \
			  } \
			  setCursor(x, y);
#define PUTC(c) video_mem[(x++) + (y*80)] = ( c | (color << 8) );
			  
int putchar(char chr) {
  PUTC(chr);
  VIDEOTEXT_CONTROL;
  return 0;
}

int puts(const char *str) {
  int i;
  for(i = 0; str[i] != '\0'; i++) {
    switch(str[i]){
      case '\n':
	y ++; x = 0;
	break;
      case '\t':
	while(++x % TABULATOR_SIZE != 0);
	break;
      default:
	PUTC(str[i]);
	break;
    }
    VIDEOTEXT_CONTROL;
  }
  return i;
}

void scroll(void) {
  y --;
  
  int i;
  for(i = 0; i < (80 * 24 * 2); i++) {
    video_mem[i] = video_mem[i + 80 * 2];
  }
  for(i = 0; i < (80 * 2); i++) {
    video_mem[i + (80 * 24 * 2)] = 0;
  }
}

void setCursor(int x, int y) {
  uint16_t tmp;
  tmp = y * VIDEOTEXT_WIDTH + x;
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
  color = ( (color&0x0f) | (bcolor << 8) );
}

void clearscreen(void) {
  int i;
  for( i = 0; i < (VIDEOTEXT_WIDTH * VIDEOTEXT_HEIGHT); i++){
    video_mem[i] = 0;
  }
}
