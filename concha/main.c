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
#include <unistd.h>
#include <string.h>

#include "concha.h"

void main(void) {
  char buf[100];
  char path[100];
  
  while(1) {
    memclr(buf, 100);
    memclr(path, 100);

    getcwd(path, 100);
    printf("%s$ ", path);
    gets(buf);
    parse_cmd(buf);
  }
}

void parse_cmd(char *str) {
  sh_cmd_t commands[] = {
    {"cd",   &command_cd},
    {"help", &command_help},
    {"exit", &command_exit}
  };
  int num_cmd = 3;
  int i=0, j=0, k=0;
  
  char cmd_str[100];
  char arg_str[100];
  int argc = 0;
  
  while(*str == ' ' || *str == '\t') { str++; } // remove spaces and tabulators
  if(str[0] == '#') { // ignore comments
//    printf("Kommentar\n");
    return;
  }
  
  while(str[i] != ' ' && str[i] != '\t' && str[i] != '\0') {i++;}
  j = i;
  while(str[j] != '\0') {j++;}
  memcpy(cmd_str, str, i);
  memcpy(arg_str, str+i, j);
  
  cmd_str[i] = '\0';
  arg_str[j] = '\0';
  if(cmd_str[0] == '\0') { // nothing entered
    return;
  }
  
  int found = 0;
  for(i = 0; i < num_cmd; i++) {
    if( strcmp(cmd_str, commands[i].name) ) {
      
      for(j = 0; arg_str[j] != '\0'; j++) {
	if(arg_str[j] == ' ' || arg_str[j] == '\t') {
	  argc++;
	}
      }
      
      char *argv[argc];
      
      for(k = 0, j = 0; k < argc; k++) {
	size_t len = 0;
	while(arg_str[j+1] != ' ' && arg_str[j+1] != '\t' && arg_str[j+1] != '\0') { j++; len++; }
        argv[i] = malloc(len);
        memcpy(argv[i], arg_str+1, len);
        argv[i][len] = '\0';
      }
      
      commands[i].handler(argc, argv);
      found = 1;
      break;
    }
  }
  
  if(!found) {
    int fd;
    if( (fd = open(cmd_str, O_RDWR, 0)) > 0 ) {
      close(fd);
      
      pid_t pid = fork();
      if(!pid) {
        exec(cmd_str);
      } else {
        waitpid(pid);
      }
    } else {
      printf("Unbekannter Befehl \'%s\'!\n\'help\' fuer Hilfe\n", cmd_str);
    }
  }
}

