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

#include "concha.h"

extern char *path;

int command_cd(uint8_t argc, uint8_t **argv) {
  if(argc > 0) {
    int ret = chdir(argv[0]);
    if(ret < 0) {
      if(ret == -1) {
	printf("\'%s\' ist kein Verzeichnis!\n", argv[0]);
      } else {
	printf("Verzeichnis nicht gefunden!\n");
      }
    } else {
//       printf("nach \'%s\' gewechselt.\n", argv[0]);
    }
  } else {
    printf("Kein Pfad angegeben!\n");
  }
  return 0;
}

int command_help(uint8_t argc, uint8_t **argv) {
  printf(
    "Concha - Help\n"
    "\tcd - Verzeichnich wechseln\n"
    "\thelp - diese Hilfe\n"
    "\texit - shell verlassen\n"
  );
  return 0;
}

int command_exit(uint8_t argc, uint8_t **argv) {
  printf("exiting concha...\n");
  exit(0);
  return 0;
}
