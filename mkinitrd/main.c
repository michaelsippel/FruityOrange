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
  
  FILE *dest = fopen(dest_path, "wa");
  if(!dest) {
    printf("Couldn't open \'%s\'!\n", dest_path);
    return EXIT_FAILURE;
  }
  
  id_t count = 0;
  struct dirent *dirent;  
  
  vfs_inode_t *inodes = malloc(sizeof(vfs_inode_t));
  while( ( dirent = readdir(parent) ) != NULL ) {
    if( (!strcmp("." ,dirent->d_name)) ||
        (!strcmp("..",dirent->d_name))
    ) {
    	continue;
    }
    printf("Reading %s...\n", dirent->d_name);
    
    struct stat attr; 
    stat(dirent->d_name, &attr);
    
    //strcpy(inodes[count].name, "123");
    
    //inodes[count].
    
    inodes = realloc(inodes, (++count)*sizeof(vfs_inode_t));
  }
  
  #define MODE S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH
  vfs_inode_t *root = malloc(sizeof(vfs_inode_t));
  strcpy(root->name, "root");
  root->parent = NULL;
  root->length = (count-1) * sizeof(vfs_inode_t);
  root->stat.mode = S_MODE_DIR | MODE;
  root->stat.id = 0;
  
  printf("\nWriting root...\n");
  fseek(dest, 0, SEEK_SET);
  fwrite(root, 1, sizeof(vfs_inode_t), dest);
  
  int i;
  for(i = 0; i < count-1; i++) {
    printf("Writing %s (%d)...\n", inodes[i].name, i);
    fwrite(&inodes[i], 1, sizeof(vfs_inode_t), dest);
  }
  
  return EXIT_SUCCESS;
}

