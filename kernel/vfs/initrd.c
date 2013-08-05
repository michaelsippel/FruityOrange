/**
 *  kernel/vfs/inode.c
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
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include <driver/console.h>
#include <vfs.h>

static void *initrd_ptr;

void initrd_read_dir(initrd_dentry_t *entries, int num, vfs_inode_t *vfs_parent) {
  int i;
  
  for(i = 0; i < num; i++) {
    initrd_inode_t *ino = initrd_ptr + entries[i].off;
    char *name = malloc(strlen((char*)ino->name)+1);
    strcpy(name, ino->name);
    
    vfs_inode_t *vfs_ino = vfs_create_inode(name, ino->mode, vfs_parent);
    vfs_ino->length = ino->length;
    vfs_ino->stat.size = ino->length;
    
    void *file_start = initrd_ptr + ino->off;
    
    if(ino->mode & S_MODE_DIR) {
      int d_num = ino->length / sizeof(initrd_inode_t);
      initrd_dentry_t *d_entries = file_start;
      
      //initrd_read_dir(d_entries, d_num, vfs_ino);
    } else {
      vfs_ino->base = file_start;
    }
  }
}

void vfs_load_initrd(void *initrd) {
  initrd_ptr = initrd;
  
  initrd_inode_t *initrd_root = initrd;
  int num = initrd_root->length / sizeof(initrd_inode_t);
  initrd_dentry_t *entries = initrd + sizeof(initrd_inode_t);
  
  initrd_read_dir(entries, num, vfs_root());
}

