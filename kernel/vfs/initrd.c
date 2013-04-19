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

#include <driver/console.h>
#include <vfs.h>

static void *initrd_ptr;

void initrd_read_dir(initrd_dentry_t *entries, int num) {
  int i;
  for(i = 0; i < num; i++) {
    initrd_inode_t *ino = initrd_ptr + entries[i].off;
    printf("\t%c %s (%d, %d)\n", ((ino->mode & S_MODE_DIR) ? 'd' : '-'), ino->name, ino->off, entries[i].off);
  }
  
  for(i = 0; i < num; i++) {
    initrd_inode_t *ino = initrd_ptr + entries[i].off;
    if(ino->mode & S_MODE_DIR) {
      int d_num = ino->length / sizeof(initrd_inode_t);
      initrd_dentry_t *d_entries = initrd_ptr + entries[0].off + sizeof(initrd_inode_t)*num;
      printf("\ninode \'%s\' (%d): %d bytes, %d file(s)\n", ino->name, ino->off, ino->length, d_num);
      
      initrd_read_dir(d_entries, d_num);
    }
  }
}

void vfs_load_initrd(void *initrd) {
  printf("Loading initial ramdisk...\n");
  initrd_ptr = initrd;  
  
  initrd_inode_t *initrd_root = initrd;
  int num = initrd_root->length / sizeof(initrd_inode_t);
  printf("inode \'%s\' (%d): %d bytes, %d file(s)\n", initrd_root->name, initrd_root->off, initrd_root->length, num);
  
  initrd_dentry_t *entries = initrd + sizeof(initrd_inode_t);
  initrd_read_dir(entries, num);
  
  printf("\n");
}

