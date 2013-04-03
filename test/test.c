/**
 *  test/test.c
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
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

int main(void) {
  int fd = open("/foo.txt", O_RDWR | O_APPEND, 0);
  if(write(fd, "Hallo Welt!", 13) > 0) {
    char buf[13];
    lseek(fd, 0, SEEK_SET);
    if(read(fd, buf, 13) > 0) {
      printf("%s\n", buf);
    } else {
      printf("Fehler beim lesen!\n");
    }
  } else {
    printf("Fehler beim schreiben!\n");
  }
  
  printf("Druecke eine Taste um zu starten...\n");
  getch();
  
  char name[100];
  char s_alter[3];
  char y_n;
  unsigned int alter;
  printf("\nWie heisst du?\n\t");
  gets(name);
  printf("Angenehm!\nWie alt bist du?\n\t");
  gets(s_alter);
  alter = atoi(s_alter);
  
  printf("Hattest du dieses Jahr schon Geburtstag? [j/n]\n\t");
  y_n = getch();
  
  if(y_n == 'j' || y_n == 'J') {
    y_n = 0;
    printf("ja\n");
  } else {
    y_n = 1;
    printf("nein\n");
  }
  
  time_t t = time();
  tm_t tm = gmtime(t);
  
  printf("\nDu heisst %s und bist %u Jahre alt!\n", name, alter);
  printf("Es ist das Jahr %d. Du bist also %d geboren!\n\n", tm.year, tm.year-alter-y_n);
  
  char text[] = "Hallo! Das ist jetzt ein Text,\n"
		"der mal so Buchstabe fuer Buchstabe angezeigt wird.\n"
		"Jetzt kommt gleich die Zeit...\n";
  
  int i;
  for(i = 0; i < sizeof(text); i++) {
    usleep(100000);
    printf("%c", text[i]);
  }
  
  char s[] = "\t%d:%d:%d (UTC)";
  char new_s[] = "\t%d:%d:%d (UTC)";
  int length = sizeof(s)-1;
  printf("\n%s", s);
  while(1) {
    t = time();
    tm = gmtime(t);
    
    for(i=0; i < length; i++) printf("\r");
    length = sprintf(new_s, s, tm.hour, tm.min, tm.sec);
    puts(new_s);
    sleep(1);
  }
  
  return 0;
}
