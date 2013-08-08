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
#include <stdio.h>
#include <stdlib.h>

#include "concha.h"

int main(int argc, char **argv) {
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

  return 0;
}

void parse_cmd(char *str) {
  sh_cmd_t commands[] = {
    {"cd",   &command_cd},
    {"cls",  &command_cls},
    {"help", &command_help},
    {"exit", &command_exit}
  };
  int num_cmd = 4;
  int i=0, j=0, k=0;
  
  char cmd_str[100];
  char arg_str[100];
  int argc = 1;
  
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
  
  for(j = 0; arg_str[j] != '\0'; j++) {
    if(arg_str[j] == ' ' || arg_str[j] == '\t') {
      argc++;
    }
  }
  
  uint8_t **argv = calloc(sizeof(uint8_t*), argc);
  char del[] = " ";
  argv[0] = (uint8_t*) strtok(str, del);
  for(k = 1; k < argc; k++) {
    argv[k] = (uint8_t*) strtok(NULL, del);
  }
  
  while( (argc > 0) && argv[argc - 1] == NULL) {
    argc--;
  }
  
  int found = 0;
  for(i = 0; i < num_cmd; i++) {
    if( strcmp((const char*)cmd_str, (char*)commands[i].name) ) {
      commands[i].handler(argc, argv);
      found = 1;
      break;
    }
  }
  
  if(!found) {
    int fd;
    fd = open(cmd_str, O_RDONLY, 0);
    if(fd < 0) {
      char bin_path[100];
      strcpy(bin_path, "/bin/");
      strcat(bin_path, cmd_str);
      strcpy(cmd_str, bin_path);
      fd = open(cmd_str, O_RDONLY, 0);
    }
    
    if(! (fd < 0) ) {
      close(fd);
      
      pid_t pid = fork();
      if(!pid) {
        exec(cmd_str, argc, (char**)argv);
      } else {
        waitpid(pid);
      }
      //pid_t pid = exec_extern(cmd_str, argc, argv);
      //waitpid(pid);
    } else {
      printf("Unknown command \'%s\'!\n\'help\' for help\n", cmd_str);
    }
  }
}

