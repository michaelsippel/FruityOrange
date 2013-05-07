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

char text[24][81];
int key = 0;
int x = 0, y = 0;

#define NEWLINE y++; x = 0;
#define TAB_SIZE 8

int main(void) {
  int i;
  
  while(key != 'q') {
    puts("\033[2J");
    print();
    
    key = getch();    

    switch(key) {
      case '\n': // newline
        NEWLINE;
        break;
      case '\t':
        for(i = 0; i < TAB_SIZE; i++) {
          text[y][x++] = ' ';
        }
      case '\r': // delete
        text[y][--x] = ' ';
        break;
      default:
        text[y][x++] = key;
        break;
    }
    
    if(x >= 80) {
      NEWLINE;
    } else if(x < 0) {
      if(y > 0) {
        x = 79;
        y--;
      } else {
        x = 0;
      }
    }
  }
  
  printf("\n");
  return 0;
}

void print(void) {
  printf("\033[6;15mTractius                                                                [%d|%d] \n", x, y);
  puts("\033[7m");
  int i;
  for(i = 0; i < 23; i++) {
    printf("%s\n", text[i]);
  }
  printf("\033[%d;%dH", y+2, x+1);
}

