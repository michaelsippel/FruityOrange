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

void vfs_load_initrd(void *initrd) {
  printf("Loading initial ramdisk...\n");
  
  initrd_inode_t *initrd_root = initrd;
  int num = initrd_root->length / sizeof(initrd_inode_t);
  printf("inode \'%s\' (%d): %d bytes, %d files\n", initrd_root->name, initrd_root->id, initrd_root->length, num);
  
  initrd_dentry_t *entries = initrd + sizeof(initrd_inode_t);
  initrd_inode_t *inodes = initrd + sizeof(initrd_inode_t) + sizeof(initrd_dentry_t)*num;
  int i;
  for(i = 0; i < num; i++) {
    printf("\t%c %s\n", ((inodes[i].mode & S_MODE_DIR) ? 'd' : '-'), entries[i].name);
  }
}

