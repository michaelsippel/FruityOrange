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

char path[256];
char text[23][81];
int x = 0, y = 0;

#define NEWLINE y++; x = 0;
#define TAB_SIZE 8

void insert_char(int key) {
  int i;
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

void save(int fd) {
  if(fd == -1) {
    fd = open(path, O_RDWR | O_CREAT, 0);
  }
  
  // TODO
}

int main(int argc, char **argv) {
  int fd = -1, c;
  int key = 0;
  
  if(argc > 0) {
    strcpy(path, argv[0]);
    fd = open(argv[0], O_RDWR | O_CREAT, 0);
    if(fd >= 0) {
      do {
        read(fd, &c, 1);
        insert_char(c);
      } while(c);
    } else {
      printf("\033[4;15mtractius: File not found!\033[0;7m\n");
      return;
    }
  } else {
    strcpy(path, "Untitled.txt");
  }
  
  while(1) {
    puts("\033[2J");
    print();
    
    key = getch();
    if(key != 0) {  
      insert_char(key);
    } else {
      key = getch();
      switch(key) {
        case 'q':
          printf("\033[25;1H\n");
          return 0;
        case 's':
          save(fd);
          break;
      }
    }
  }
  
  printf("\033[25;1H\n");
  close(fd);
  
  return 0;
}

void print(void) {
  int i;
  char str[80];
  sprintf(str, "[%s|%d|%d]", path, x, y);
  printf("\033[6;15m Tractius                                                                       \n");
  printf("\033[1;%dH%s\n", 80-strlen(str), str);
  
  puts("\033[7m");
  for(i = 0; i < 23; i++) {
    printf("%s\n", text[i]);
  }
  printf(" \033[6;15m AltGr  +q-Quit  +s-Save                                                      \033[0;7m");
  printf("\033[%d;%dH", y+2, x+1);
}

