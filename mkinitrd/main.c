/**
 *  mkinitrd/main.c
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

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

int main(int argc, char **argv) {
  if(argc != 3) {
    printf("Usage: %s [dest] [src]!\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  char *dest_path = argv[1];
  char *src_path = argv[2];  
  
  printf("Creating initial ramdisk to \'%s\' from \'%s\'...\n", dest_path, src_path);  
  
  DIR *parent = opendir(src_path);
  if(!parent) {
    printf("Couldn't open \'%s\'!\n", src_path);
    return EXIT_FAILURE;
  }
  
  FILE *dest = fopen(dest_path, "wb");
  if(!dest) {
    printf("Couldn't open \'%s\'!\n", dest_path);
    return EXIT_FAILURE;
  }
  
  id_t count = 0;
  struct dirent *dirent;  
  
  int parent_id = 0;
  initrd_inode_t *inodes = calloc(4, sizeof(initrd_inode_t));//FIXME
  chdir(src_path);
  while( ( dirent = readdir(parent) ) != NULL ) {
    if( (!strcmp("." ,dirent->d_name)) ||
        (!strcmp("..",dirent->d_name))
    ) {
    	continue;
    }
    printf("Reading %s...\n", dirent->d_name);
    
    struct stat attr; 
    stat(dirent->d_name, &attr);
    
    if(S_ISDIR(attr.st_mode)) {
      inodes[count].mode = S_MODE_DIR;
    }
    
    strcpy(&inodes[count].name, dirent->d_name);
    
    inodes[count].id = count;
    inodes[count].parent_id = parent_id;
    
    count++;
//    inodes = realloc(inodes, (1+count++)*sizeof(initrd_inode_t*));
  }
  
  int i;
  printf("\nGenerating directory entries...\n");
  initrd_dentry_t *root_dentries = calloc(count, sizeof(initrd_dentry_t));
  for(i = 0; i < count; i++) {
    strcpy(root_dentries[i].name, inodes[i].name);
    root_dentries[i].id = i;
  }
  
  initrd_inode_t *root = malloc(sizeof(initrd_inode_t));
  strcpy(root->name, "root");
  root->mode = S_MODE_DIR;
  root->id = 0;
  root->length = count * sizeof(initrd_inode_t);
  printf("%d bytes\n", root->length);
  printf("\nWriting root...\n");
  fwrite(root, sizeof(initrd_inode_t), 1, dest);
  fwrite(root_dentries, sizeof(initrd_dentry_t), count, dest);
  
  for(i = 0; i < count; i++) {
    printf("Writing %s (%d)...\n", inodes[i].name, inodes[i].id);
    fwrite(&inodes[i], sizeof(initrd_inode_t), 1, dest);
  }
  
  return EXIT_SUCCESS;
}

