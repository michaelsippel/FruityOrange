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
#include <stdio.h>
#include <string.h>

#include "concha.h"

char *path;

void main(void){
  char buf[100];
  path = "/";
  sh_cmd_t *cmd;
  
  while(1){
    printf("%s%s", path, SHELL_SYMBOL);
    gets(buf);
    cmd = parse_cmd(buf);
    if(cmd != NULL) {
      cmd->handler(0);
    } else {
      printf("Unbekannter Befehl!\n\"help\" fuer Hilfe\n");
    }
  }
}

sh_cmd_t *parse_cmd(char *str) {
  sh_cmd_t commands[] = {
    {"cd",   &command_cd},
    {"help", &command_help},
    {"exit", &command_exit}
  };
  int num_cmd = 3;
  
  sh_cmd_t *cmd;
  
  int i;
  for(i = 0; i < num_cmd; i++) {
    if( strcmp(str, commands[i].name) ) {
      cmd = &commands[i];
      return cmd;
    }
  }
  
  return NULL;
}
