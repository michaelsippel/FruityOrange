/**
 *  kernel/driver/pit/pit.c
 *
 *  (C) Copyright 2013 Michael Sippel
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
#include <driver/pit.h>

static unsigned int pit_freq = 0;

void init_pit(int freq) {
  pit_freq = freq;
  int count = 1193182 / freq;
  
  outb(0x43, 0x34);
  outb(0x40, count & 0xFF);
  outb(0x40, count >> 8);
}

unsigned int pit_get_freq(void) {
  return pit_freq;
}
