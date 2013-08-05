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

#define SIZE_X 80
#define SIZE_Y 23
char display_text[SIZE_Y][SIZE_X+1];

#define TEXT_BLOCK_SIZE 0x1000
int text_length = 0;
int text_blocks = 0;
char *text;
int x = 0, y = 0;

#define TAB_SIZE 8

void print(void);
void insert_char(char c);
void insert_key(int key);
void save(int fd);

int main(int argc, char **argv) {
  int fd = -1, c;
  int key = 0;
  
  text = malloc(TEXT_BLOCK_SIZE);
  text_blocks = 1;
  
  if(argc > 1) {
    strcpy(path, argv[1]);
    fd = open(argv[1], O_RDWR | O_CREAT, 0x1ff0);
    if(fd >= 0) {
      stat_t stat;
      fstat(fd, &stat);
      int len = stat.size;
      while(len > 0) {
        read(fd, &c, 1);
        insert_key(c);
        len--;
      }
    } else {
      printf("\033[4;15mtractius: File not found!\033[0;7m\n");
      return -1;
    }
  } else {
    strcpy(path, "Untitled.txt");
  }
  
  while(1) {
    puts("\033[2J");
    print();
    
    key = getch();
    if(key != 0) {  
      insert_key(key);
    } else {
      key = getch();
      switch(key) {
        case 'q':
          printf("\033[2J");
          if(fd >= 0) close(fd);
          return 0;
        case 's':
          save(fd);
          break;
      }
    }
  }  
  
  return -1;
}

void print(void) {
  int i;
  char str[80];
  sprintf((char*)str, "[%s|%d|%d]", path, x, y);
  printf("\033[6;15m Tractius                                                                       \n");
  printf("\033[1;%dH%s\n", 80-strlen(str), str);
  
  puts("\033[7m");
  for(i = 0; i < 23; i++) {
    printf("%s\n", display_text[i]);
  }
  printf(" \033[6;15m AltGr  +q-Quit  +s-Save                                                      \033[0;7m");
  printf("\033[%d;%dH", y+2, x+1);
}

void insert_char(char c) {
  static int old_blocks = 0;
  text_length++;

  old_blocks = text_blocks;
  text_blocks = text_length / TEXT_BLOCK_SIZE;
  if(text_blocks > text_blocks)
    text = realloc(text, text_blocks * TEXT_BLOCK_SIZE);


  y += x / SIZE_X;
  x %= SIZE_X;
  if(c != '\n') display_text[y][x] = c;
  text[text_length-1] = c;
}

void insert_key(int key) {
  int i;
  switch(key) {
    case '\n': // newline
      insert_char('\n');
      y++;
      x = 0;
      break;
    case '\t':
      for(i = 0; i < TAB_SIZE; i++) {
        insert_char(' ');
        x++;
      }
      break;
    case '\r': // delete
      x--;
      insert_char(' ');
      break;
    default:
      insert_char(key);
      x++;
      break;
  }
}

void save(int fd) {
  if(fd == -1) {
    fd = open(path, O_RDWR | O_CREAT, 0x1ff0);
  }

  int i;
  for(i = 0; i < text_length; i++) {
    write(fd, text, 1);
    text++;
  }  

  // TODO
}

