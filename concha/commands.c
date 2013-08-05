/**
 *  concha/commands.c
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
#include <unistd.h>

#include "concha.h"

extern char *path;

int command_cd(uint8_t argc, uint8_t **argv) {
  if(argc > 1) {
    int ret = chdir((const char*)argv[1]);
    if(ret < 0) {
      if(ret == -1) {
	printf("\'%s\' is no directory!\n", argv[1]);
      } else {
	printf("No such file or directory!\n");
      }
    }
  }
  
  return 0;
}

int command_cls(uint8_t argc, uint8_t **argv) {
  printf("\033[2J");
  
  return 0;
}

int command_help(uint8_t argc, uint8_t **argv) {
  printf(
    "Concha - Help\n"
    "\tcd - change directory\n"
    "\tcls - clear screen\n"
    "\thelp - this help\n"
    "\texit - exit shell\n"
  );
  return 0;
}

int command_exit(uint8_t argc, uint8_t **argv) {
  printf("exiting concha...\n");
  exit(0);
  return 0;
}
