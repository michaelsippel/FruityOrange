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
#include <tar.h>

#include <driver/console.h>
#include <vfs.h>

void vfs_load_initrd(void *initrd) {
  int num = tar_get_num_entries(initrd);
  tar_header_t *headers[num];
  
  int i;
  for(i = 0; i < num; i++) {
    headers[i] = initrd;
    
    int size = tar_getsize(headers[i]->size);

    if(i == 0) goto next;
    
    char *path = headers[i]->name+1;
    int path_len = strlen(path);
    
    char *filename = NULL;
    
    char del[] = "/";
    char *str = strtok(path, del);
    
    while(str != NULL) {
      if(str[0] != '\0') {
        filename = str;
      }
      str = strtok(NULL, del);
    }
    
    char parent_path[100];
    int parent_len = path_len - strlen(filename);
    if(path[path_len-1] == '/') parent_len--;
    memcpy(parent_path, path, parent_len);
    parent_path[parent_len] = '\0';
    
    printf("%s;%s;%s\n", path, parent_path, filename);
    vfs_inode_t *parent_inode = vfs_path_lookup(parent_path);
    
    if(path[path_len-1] == '/') { // DIR
      vfs_create_inode(filename, S_MODE_DIR | 0x1ff0, parent_inode);
    } else { // Regular
      vfs_inode_t *inode = vfs_create_inode(filename, 0x1ff0, parent_inode);
      vfs_write(inode, 0, initrd + TAR_SIZE, size);
    }
    
next:
    initrd += ((size / TAR_SIZE) + 1) * TAR_SIZE;
    
    if (size % TAR_SIZE) {
      initrd += TAR_SIZE;
    }
  }
}

