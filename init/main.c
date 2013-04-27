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
  printf("Init\n");
  
  pid_t pid = fork();
  if(!pid) {
    printf("starting shell...\n");
    exec("/concha");
  } else {
    waitpid(pid);
    printf("shell exited.\n");
  }
  
  return 0;
}

