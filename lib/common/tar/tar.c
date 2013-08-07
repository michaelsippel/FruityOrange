/**
 *  lib/common/tar.c
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
#include <tar.h>

int tar_getsize(const char *in) {
  int size = 0;
  int i;
  int count = 1;
  
  for (i = 11; i > 0; i--, count *= 8) {
    size += ((in[i - 1] - '0') * count);
  }
  
  return size;
}

int tar_get_num_entries(void *addr) {
  int i = -1;
  
  tar_header_t *header = (tar_header_t*)addr;
  while(header->name[0]) {
    header = (tar_header_t*)addr;
    int size = tar_getsize(header->size);
    addr += ((size / TAR_SIZE) + 1) * TAR_SIZE;
    if(size % TAR_SIZE) {
      addr += TAR_SIZE;
    }
    i++;
  }
  
  return i;
}

