/**
 *  lib/common/string/string.c
 *
 *  (C) Copyright 2012 Michael Sippel
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
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <string.h>

size_t strlen(char *s){
    int i;
    for(i=0;*s!='\0';s++,i++);
    return(i);
}

void cut(char *s,int pos,char *os1,char *os2){
  int i = 0;
  while(i<pos) {*os1++ = *s++;i++;}
  os1[i] = '\0';
  while(*s !='\0') {*os2++ = *s++;}
  os2[i] = '\0';
}

void insert(char *s,int pos,char *is){
  size_t i,j;
  char *bufferU = 0,*buffer = 0;
  cut(s,pos,bufferU,buffer);
  for(i=0,j=pos;i<strlen(is);i++,j++)s[j] = is[i];
  for(i=0;buffer[i]!='\0';i++,j++)s[j] = buffer[i];
}

void replace(char *s,int pos, char *rs){
  int i,j;
  for(i=pos,j=0;rs[j]!='\0';i++,j++)s[i]=rs[j];
  s[i] = '\0';
}

void strcpy(char *dest, const char *src){
  while((*dest++ = *src++));
}

char *strtok(char *string, char *delimiters) {
  static char *s = NULL;
  static int num_del = 0;
  if(string != NULL) {
    s = string;
    num_del = strlen(delimiters);
  } else {
    if(s == NULL) {
      return NULL;
    }
    string = s;
  }
  int i, j = 0;
  while(*s != '\0') {
    for(i = 0; i < num_del; i++) {
      if(*s == delimiters[i]) {
        s++;
        char *ret = malloc(j+1);
        memcpy(ret, string, j);
        ret[j] = '\0';
        return ret;
      }
    }
    s++;
    j++;
  }
  
  char *ret = malloc(j+1);
  strcpy(ret, string);
  
  s = NULL;
  return ret;
}

char *strcat(char *dest, const char *src){
    while (*dest) dest++;
    
    do{
        *dest++ = *src++;
    }while (*src != 0);
    
    return dest;
}

int strcmp(char *str1, char *str2){
      int i = 0;
      while(str1[i] != '\0' && str2[i] != '\0'){
          if(str1[i] != str2[i]){
              return FALSE;
              break;
          }
          i++;
      }
      if( (str1[i] == '\0' && str2[i] != '\0') || (str1[i] != '\0' && str2[i] == '\0') )
	return FALSE;
      else
	return TRUE;
}
