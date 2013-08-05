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
  if(argc < 1) {
    printf("Usage: %s [file 1] [file 2] ...\n", "cat");//argv[0]);
    return -1;
  }
  /*
  int i;
  stat_t stat;
  //for(i = 0; i < argc; i++) {
    printf("%x\n", &argv);
    fd_t f = open(argv[0], O_RDONLY, 0);
    if(f < 0) {
      printf("File not Found(%d)!\n", 0);
      //continue;
    }
    //printf("asd");
    fstat(f, &stat);
    size_t len = stat.size;
    
    char c;
    do {
      read(f, &c, 1);
      printf("%c", c);
      len--;
    } while(c);
    
    close(f);
  //}
  */
  return 0;
}

