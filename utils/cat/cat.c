/**
 *  utils/ls/ls.c
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
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv) {
  if(argc < 1) {
    printf("Usage: %s [file]\n", "cat");//argv[0]);
    return -1;
  }
  
  int f = open(argv[0], O_RDONLY, 0);// TODO
  if(f == NULL) {
    printf("Error.\n");
    return -1;
  }
  
  char c;
  int len = 13;// TODO
  int i;
  for(i = 0; i < len; i++) {
    read(f, &c, 1);
    printf("%c", c);
  }
  
  printf("\n");
  close(f);
  
  return 0;
}

