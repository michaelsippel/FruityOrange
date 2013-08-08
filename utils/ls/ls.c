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

int main(int argc, char **argv) {
  int d;
  if(argc > 1) {
    d = open(argv[1], O_RDONLY, 0);
  } else {
    d = open(".", O_RDONLY, 0);
  }
  
  if(d < 0) {
    printf("File not found!\n");
    return -1;
  }
  
  dirent_t *dentry;
  while( dentry = readdir(d) ) {
    int color = S_ISDIR(dentry->stat) ? 0x9 : 0xF;
    printf("%c%c%c%c%c%c%c%c%c%c \033[0;%dm%s\033[0;7m\n",
	   (S_ISDIR(dentry->stat) ? 'd' : '-'),
	   ((dentry->stat.mode & S_IRUSR) ? 'r' : '-'),
	   ((dentry->stat.mode & S_IWUSR) ? 'w' : '-'),
	   ((dentry->stat.mode & S_IXUSR) ? 'x' : '-'),
	   
	   ((dentry->stat.mode & S_IRGRP) ? 'r' : '-'),
	   ((dentry->stat.mode & S_IWGRP) ? 'w' : '-'),
	   ((dentry->stat.mode & S_IXGRP) ? 'x' : '-'),
	   
	   ((dentry->stat.mode & S_IROTH) ? 'r' : '-'),
	   ((dentry->stat.mode & S_IWOTH) ? 'w' : '-'),
	   ((dentry->stat.mode & S_IXOTH) ? 'x' : '-'),
	   color,
	   dentry->name
    );
  }
  
  close(d);
  
  return 0;
}

