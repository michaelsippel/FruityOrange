/**
 *  kernel/vfs/pathname_lookup.c
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
#include <stdint.h>
#include <stdbool.h>

#include <mm.h>
#include <vfs.h>
#include <proc/scheduler.h>

vfs_inode_t *vfs_path_lookup(const char *path) {
  vfs_inode_t *parent = vfs_root();
  vfs_inode_t *inode = NULL;
  
  int i,j,ino;
  
  if(path[0] != '/') {
    parent = current_proc->work_dir;
    printf("%s\n", parent->name);
    i = 0;
  } else {
    i = 1;
  }
  
  vfs_dentry_t *entries = vfs_read(parent, 0);
  int num = parent->length / sizeof(vfs_dentry_t);
  bool *map = malloc(num);
  
  for(j = 0; j < num; j++) map[j] = TRUE;
  
  j = 0;
  ino = 0;
  
  while(path[i] != '\0') {
    switch(path[i]) {
      case '/':
	for(i = 0; i < num; i++) {
	  if(map[i] == TRUE) {
	    parent = entries[i].inode;
	  }
	}
	entries = vfs_read(parent, 0);
	num = parent->length / sizeof(vfs_dentry_t);
	map = realloc(map, num);
	for(j = 0; j < num; j++) map[j] = TRUE;
	j = 0;
	i++;
	break;
	
      default:
	for(ino = 0; ino < num; ino++) {
	  inode = entries[ino].inode;
	  if(map[ino]) {
	    if(path[i] != inode->name[j] && inode->name[j] != '\0') {
	      map[ino] = FALSE;
	    }
	  }
	}
	j++;
	break;
    }
    i++;
  }
  
  for(i = 0; i < num; i++) {
    if(map[i] == TRUE) {
      inode = entries[i].inode;
      return inode;
    }
  }
  
  return NULL;
}
