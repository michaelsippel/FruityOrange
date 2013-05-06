/**
 *  tractius/main.c
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
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

char text[25][80];

int main(void) {
  int key = 0;
  int x = 0, y = 0;
  
  while(key != 'q') {
    clear();
    print();
    
    key = getch();
    text[y][x++] = key;
  }
  printf("\n");
  return 0;
}

void clear(void) {
  puts("\033[2J");
}

void print(void) {
  printf("\033[6;15mTractius                                                                        \n");
  puts("\033[7m");
  int i;
  for(i = 0; i < 25; i++) {
    printf("%s", text[i]);
  }
}

