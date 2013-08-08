/**
 *  concha/main.c
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
#include <sys/syscalls.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
  time_t t = time();
  tm_t tm = gmtime(t);
  printf("\033[0;6mWelcome to FruityOrange!\033[0;7m\n"
         "%d/%d/%d\n"
         "%d:%d:%d (UTC)\n", 
         tm.mday, tm.mon, tm.year, 
         tm.hour, tm.min, tm.sec);

  /*
  pid_t pid = fork();
  if(!pid) {
    exec("/concha", 0, 0);
  } else {
    waitpid(pid);
    printf("shell exited.\n");
  }
  */
  pid_t pid = exec_extern("/bin/concha", 0, 0);
  waitpid(pid);
  printf("shell exited.\n");  

  return 0;
}

