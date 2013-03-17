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
  printf("Druecke eine Taste um zu starten...\n");
  getch();
  
  char name[100];
  char s_alter[3];
  unsigned int alter;
  printf("\nWie heisst du?\n\t");
  gets(name);
  printf("Angenehm!\nWie alt bist du?\n\t");
  gets(s_alter);
  alter = atoi(s_alter);
  
  time_t t = time();
  tm_t tm = gmtime(t);
  
  printf("\nDu heisst %s und bist %u Jahre alt!\n", name, alter);
  printf("Es ist das Jahr %d.Du bist also %d geboren!\n\n", tm.year, tm.year-alter);
  
  char s[] = "%d:%d:%d (UTC)";
  printf("\n%s", s);
  while(1) {
    int i;
    t = time();
    tm = gmtime(t);
    
    for(i=0; i < sizeof(s)-1; i++) printf("\r");
    printf(s, tm.hour, tm.min, tm.sec);
    sleep(1);
  }
  
  return 0;
}
