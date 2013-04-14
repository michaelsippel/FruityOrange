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
  
  vfs_inode_t *initrd_root = malloc(sizeof(vfs_inode_t));
  strcpy(initrd_root->name, initrd);
  memcpy(&initrd_root->length, &initrd[256], 4);
  vfs_dentry_t *entries = vfs_read(initrd_root, 0);
  int num = initrd_root->length / sizeof(vfs_dentry_t);
  printf("%s, %d bytes, %d files\n", initrd_root->name, initrd_root->length, num);
}

