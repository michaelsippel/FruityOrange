/**
 *  kernel/vfs/vfs.c
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

#include <driver/cmos.h>
#include <vfs.h>
#include <mm.h>

static uint32_t id_counter = 0;
static vfs_inode_t *root = NULL;

static uid_t uid = 0;
static gid_t gid = 0;

void init_vfs(void) {
  root = malloc(sizeof(vfs_inode_t));
  root->name = "root";
  root->type = VFS_TYPE_DIR;
  root->stat.id = id_counter++;
  root->base = NULL;
  root->length = 0;
  root->parent = NULL;
}

vfs_inode_t *vfs_create_inode(const char *name, mode_t mode, vfs_inode_t *parent) {
  vfs_inode_t *inode = malloc(sizeof(vfs_inode_t));
  
  inode->name = name;
  inode->length = 0;
  
  if (parent == NULL) {
    inode->parent = root;
  } else {
    inode->parent = parent;
  }
  
  inode->stat.mode = mode;
  inode->stat.id = id_counter++;
  inode->stat.atime =
  inode->stat.mtime =
  inode->stat.ctime = mktime(mktm(get_cmos_time()));
  
  return inode;
}

int vfs_write(vfs_inode_t *inode, void *base, size_t bytes) {
  int i = 0;
  int writable = 0;
  if ((inode->stat.uid == uid) &&
      (inode->stat.mode & S_IWUSR)) 
  {
    writable = 1;
  } else if
    ((inode->stat.gid == gid) && 
     (inode->stat.mode & S_IWGRP))
  {
    writable = 1;
  } else {
    if (inode->stat.mode & S_IWOTH) {
      writable = 1;
    }
  }
  
  if (writable) {
    if (inode->base == NULL) {
	inode->base = malloc(bytes);
    } else {
	inode->base = realloc(inode->base, inode->length + bytes);
    }
    
    uint8_t *nbase = (uint8_t*) inode->base + inode->length;
    uint8_t *wbase = (uint8_t*) base;
    while (i++ < bytes) {
	*nbase++ = *wbase++;
	inode->length++;
    }
  } else {
    printf("[vfs] inode %d isn't writable!\n", inode->stat.id);
  }
  
  return i;
}

void* vfs_read(vfs_inode_t *inode, uintptr_t offset) {
	return (void*) inode->base + offset;
}

int vfs_access(vfs_inode_t *inode, mode_t modus) {
  if (inode->stat.uid == uid) 
  {
    if ((modus & R_OK) && 
        !(inode->stat.mode & S_IRUSR))
	return -1;
    if ((modus & W_OK) &&
        !(inode->stat.mode & S_IWUSR))
	return -1;
    if ((modus & X_OK) &&
        !(inode->stat.mode & S_IXUSR))
	return -1;
  } 
  else if (inode->stat.gid == gid) 
  {
    if ((modus & R_OK) &&
        !(inode->stat.mode & S_IRGRP))
	return -1;
    if ((modus & W_OK) &&
	!(inode->stat.mode & S_IWGRP))
	return -1;
    if ((modus & X_OK) &&
	!(inode->stat.mode & S_IXGRP))
	return -1;
  } 
  else 
  {
    if ((modus & R_OK) &&
        !(inode->stat.mode & S_IROTH))
	return -1;
    if ((modus & W_OK) &&
	!(inode->stat.mode & S_IWOTH))
	return -1;
    if ((modus & X_OK) &&
	!(inode->stat.mode & S_IXOTH))
	return -1;
  }
  
  return 0;
}
