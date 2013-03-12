/**
 *  lib/common/string/mem.c
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
#include <stddef.h>

#include <string.h>

void *memcpy(void *dest, const void *src, size_t n) {
  const uint8_t *sp = (const uint8_t *) src;
  uint8_t *dp = (uint8_t *) dest;
  while(n-- > 0) *dp++ = *sp++;
  
  return dest;
}

void *memmove(void *dest, const void *src, size_t n) {
  const uint8_t *sp = (const uint8_t *) src+n;
  uint8_t *dp = (uint8_t *) dest+n;
  while(n-- > 0) *dp-- = *sp--;
  
  return dest;
}

void *memset(void *addr, int value, size_t n) {
  uint8_t *tmp = (uint8_t*) addr;
  while(n-- > 0) *tmp++ = value;
  return addr;
}

void *memclr(void *addr, size_t n) {
  uint8_t *tmp = (uint8_t*) addr;
  while(n-- > 0) *tmp++ = 0;
  return addr;
}
