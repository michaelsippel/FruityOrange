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

#include <driver/cmos.h>
#include <driver/console.h>
#include <vfs.h>
#include <mm.h>

static uint32_t id_counter = 0;
static vfs_inode_t *root = NULL;

static uid_t uid = 0;
static gid_t gid = 0;

void init_vfs(void) {
  root = vfs_create_inode("root", S_MODE_DIR | S_IRUSR | S_IWUSR, NULL);
  vfs_init_syscalls();
}

vfs_inode_t *vfs_root(void) {
  return root;
}

// only debug function
void vfs_inode_list(vfs_inode_t *parent) {
  if(parent == NULL) {
    parent = root;
  }
  
  vfs_dentry_t *entries = vfs_read(parent, 0);
  int i;
  
  int num = parent->length / sizeof(vfs_dentry_t);
  printf("inode-list from parent \"%s\" (%d)\n", parent->name, parent->stat.id);
  for(i = 0; i < num; i++) {
    vfs_dentry_t dentry = entries[i];
    vfs_inode_t *inode = dentry.inode;
    stat_t stat = inode->stat;
    printf("\t%c %c%c%c %c%c%c %c%c%c %s\n",
	   (S_ISDIR(stat) ? 'd' : '-'),
	   ((stat.mode & S_IRUSR) ? 'r' : '-'),
	   ((stat.mode & S_IWUSR) ? 'w' : '-'),
	   ((stat.mode & S_IXUSR) ? 'x' : '-'),
	   
	   ((stat.mode & S_IRGRP) ? 'r' : '-'),
	   ((stat.mode & S_IWGRP) ? 'w' : '-'),
	   ((stat.mode & S_IXGRP) ? 'x' : '-'),
	   
	   ((stat.mode & S_IROTH) ? 'r' : '-'),
	   ((stat.mode & S_IWOTH) ? 'w' : '-'),
	   ((stat.mode & S_IXOTH) ? 'x' : '-'),
	   
	   dentry.inode->name
    );
  }
}

vfs_inode_t *vfs_create_inode(const char *name, mode_t mode, vfs_inode_t *parent) {
  vfs_inode_t *inode = malloc(sizeof(vfs_inode_t));
  memclr(inode, sizeof(vfs_inode_t));
  
  inode->name = name;
  inode->length = 0;
  
  if (parent != NULL) {
    if(parent->stat.mode & S_MODE_DIR) {
      inode->parent = parent;
      vfs_dentry_t *entry = vfs_create_dentry(inode);
      vfs_write(inode->parent, parent->length, entry, sizeof(vfs_dentry_t));
    } else {
      printf("[vfs] parent inode is no directory!\n");
      return NULL;
    }
  }
  
  inode->stat.mode = mode;
  inode->stat.size = inode->length;
  inode->stat.id = id_counter++;
  inode->stat.atime =
  inode->stat.mtime =
  inode->stat.ctime = mktime(mktm(get_cmos_time()));
  
  return inode;
}

vfs_dentry_t *vfs_create_dentry(vfs_inode_t *inode) {
  vfs_dentry_t *dentry = malloc(sizeof(vfs_dentry_t));
  
  dentry->id = inode->stat.id;
  dentry->inode = inode;
  
  return dentry;
}

int vfs_write(vfs_inode_t *inode, int off, const void *base, size_t bytes) {
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
    if( (off + bytes) > inode->length) {
      inode->length = off + bytes;
      inode->stat.size = inode->length;
    }
    
    if (inode->base == NULL) {
      inode->base = malloc(inode->length);
    } else {
      inode->base = realloc(inode->base, inode->length);
    }
    
    uint8_t *nbase = (uint8_t*) inode->base + off;
    uint8_t *wbase = (uint8_t*) base;
    
    while (i++ < bytes) {
      *nbase++ = *wbase++;
    }
  } else {
    printf("[vfs] inode %d isn't writable! (0x%x)\n", inode->stat.id, inode);
  }
  
  return i-1;
}

void *vfs_read(vfs_inode_t *inode, int off) {
  return (void*) inode->base + off;
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
