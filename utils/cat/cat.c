/**
 *  utils/cat/cat.c
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
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>

int main(int argc, char **argv) {
  if(argc < 2) {
    printf("Usage: %s [file 1] [file 2] ...\n", argv[0]);
    return -1;
  }
  
  int i;
  stat_t stat;
  for(i = 1; i < argc; i++) {
    int f = open(argv[i], O_RDONLY, 0);
    if(f >= 0) {
      fstat(f, &stat);
      int len = stat.size;
      char c;
      while(len > 0) {
        read(f, &c, 1);
        printf("%c", c);
        len--;
      }
      
      close(f);
    } else {
      printf("File not Found(%d, \"%s\")!\n", i, argv[i]);
    }
  }
  
  return 0;
}

