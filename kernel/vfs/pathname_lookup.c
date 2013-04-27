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
  
  if(path[0] != '/') {
    parent = current_proc->work_dir;
  } else {
    path++;
  }
  
  vfs_dentry_t *entries = vfs_read(parent, 0);
  int i, num = parent->length / sizeof(vfs_dentry_t);
  
  if(strcmp(path, ".")) return current_proc->work_dir;
  if(strcmp(path, "..")) return current_proc->work_dir->parent;
  if(strcmp(path, "/")) return vfs_root();
  
  char delimiter[] = "/";
  char *ptr;
  
  ptr = strtok(path, delimiter);
  while(ptr != NULL) {
    for(i = 0; i < num; i++) {
      //if(strcmp(ptr, entries[i].name)) {
        //inode = entries[i].inode;
        //if(S_ISDIR(inode->stat)) {
        //} else {
        //  return inode;
        //}
      //}
    }
    ptr = strtok(NULL, delimiter);
  }
  
  return NULL;
}

void vfs_generate_path(char *buf, size_t bytes, vfs_inode_t *parent) {
  size_t len = 0;
  
  vfs_inode_t *inode = parent;
  while(inode != vfs_root()) {
    len += strlen(inode->name) + 1;
    inode = inode->parent;
  }
  
  if(len > bytes) {
    printf("Puffer zu klein!\n");
    return;
  }
  
  memclr(buf, bytes);
  if(len > 0) {
    buf += len+1;
    *--buf = '\0';
    
    inode = parent;
    while(inode != vfs_root()) {
      buf -= strlen(inode->name);
      memcpy(buf, inode->name, strlen(inode->name));
      
      *--buf = '/';
      inode = inode->parent;
    }
  } else {
    strcpy(buf,"/");
  }
}
