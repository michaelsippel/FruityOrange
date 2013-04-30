/**
 *  utils/ls/ls.c
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
#include <stdio.h>

int main(void) {
  int d = open(".", O_RDONLY, 0);
  if(d == NULL) {
    printf("Error.\n");
    return -1;
  }
  
  dirent_t *dentry;
  while( dentry = readdir(d) ) {
    printf("\t%s\n", dentry->name);
  }
  
  close(d);
  
  return 0;
}
