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

#define MODE_ALL 0x1ff0

uint32_t parent_off = 0;
uint32_t gen_off = 0;
FILE *dest;
DIR *parent;

initrd_inode_t *initrd_read_dir(const char *path, int *num) {
  struct dirent *dirent;
  initrd_inode_t *inodes = calloc(100, sizeof(initrd_inode_t));//FIXME
  chdir(path);
  int i = 0;
  while( ( dirent = readdir(parent) ) != NULL ) {
    if( (!strcmp("." ,dirent->d_name)) ||
        (!strcmp("..",dirent->d_name))
    ) {
    	continue;
    }
    
    struct stat attr; 
    stat(dirent->d_name, &attr);
    
    strcpy(&inodes[i].name, dirent->d_name);
    
    inodes[i].parent_off = parent_off;
    
    inodes[i].mode = MODE_ALL;
    if(S_ISDIR(attr.st_mode)) {
      inodes[i].mode |= S_MODE_DIR;
    }
    
    i++;
  }
  *num = i;

  return inodes;
}

initrd_dentry_t *initrd_generate_dentries(initrd_inode_t *inodes, int num) {
  printf("Generating directory entries...\n");
  int i;
  initrd_dentry_t *dentries = calloc(num, sizeof(initrd_dentry_t));
  for(i = 0; i < num; i++) {
    strcpy(dentries[i].name, inodes[i].name);
    printf("\t%d - %s\n", i, dentries[i].name);
  }
  
  return dentries;
}

void initrd_write(initrd_inode_t *inodes, initrd_dentry_t *entries, int num) {
  int i;
  
  int count[num];
  initrd_inode_t *d_inodes[num];
  initrd_dentry_t *d_entries[num];  
  
  for(i = 0; i < num; i++) {
    printf("PREPEARING %s...\n", inodes[i].name);
    inodes[i].off = entries[i].off = gen_off + num*sizeof(initrd_dentry_t) + sizeof(initrd_inode_t)*i;
    
    if(inodes[i].mode & S_MODE_DIR) {
      closedir(parent);
      parent = opendir(inodes[i].name);
      parent_off = inodes[i].off;
      
      d_inodes[i] = initrd_read_dir(inodes[i].name, &count[i]);
      d_entries[i] = initrd_generate_dentries(d_inodes[i], count[i]);
      inodes[i].length = count[i] * sizeof(initrd_inode_t);
      
      closedir(parent);
      parent = opendir("..");
    }
  }
  
  printf("writing directory entries at %d\n", gen_off);
  gen_off += fwrite(entries, 1, sizeof(initrd_dentry_t) * num, dest);
  
  for(i = 0; i < num; i++) {
    printf("WRITING %s (%d)...\n", inodes[i].name, gen_off);
    gen_off += fwrite(&inodes[i], 1, sizeof(initrd_inode_t), dest);
  }
  
  printf("\n");
  
  for(i = 0; i < num; i++) {
    if(inodes[i].mode & S_MODE_DIR) {
      closedir(parent);
      parent = opendir(inodes[i].name);
      parent_off = inodes[i].off;
      
      initrd_write(d_inodes[i], d_entries[i], count[i]);
      
      closedir(parent);
      parent = opendir("..");
    }
  }
}

int main(int argc, char **argv) {
  if(argc != 3) {
    printf("Usage: %s [dest] [src]!\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  char *dest_path = argv[1];
  char *src_path = argv[2];
  
  printf("Creating initial ramdisk to \'%s\' from \'%s\'...\n", dest_path, src_path);  
  
  parent = opendir(src_path);
  if(!parent) {
    printf("Couldn't open \'%s\'!\n", src_path);
    return EXIT_FAILURE;
  }
  
  dest = fopen(dest_path, "wb");
  if(!dest) {
    printf("Couldn't open \'%s\'!\n", dest_path);
    return EXIT_FAILURE;
  }
  
  gen_off = 0;
  fseek(dest, SEEK_SET, 0);
  
  int i, root_count = 0;
  initrd_inode_t *root_inodes = initrd_read_dir(src_path, &root_count);
  
  initrd_inode_t *root = malloc(sizeof(initrd_inode_t));
  strcpy(root->name, "root");
  root->mode = S_MODE_DIR | MODE_ALL;
  root->off = 0;
  root->parent_off = 0;
  root->length = root_count * sizeof(initrd_inode_t);
  printf("%d bytes\n", root->length);
  
  printf("\nWriting root (%d bytes)...\n\n", root->length);
  gen_off += fwrite(root, 1, sizeof(initrd_inode_t), dest);
  
  initrd_dentry_t *root_dentries = initrd_generate_dentries(root_inodes, root_count);
  initrd_write(root_inodes, root_dentries, root_count);
  
  return EXIT_SUCCESS;
}

