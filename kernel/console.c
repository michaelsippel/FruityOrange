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
#include <console.h>

static uint16_t *video_mem = (uint16_t*) 0xB8000;
static uint8_t color = 0x07;
static int tp = 0;

int puts(const char *str) {
  int i;
  for(i = 0; str[i] != '\0'; i++){
    video_mem[tp++] = ( str[i] | (color << 8) );
  }
  return i;
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
  for( i = 0; i < (80 * 25); i++){
    video_mem[i] = 0;
  }
}
