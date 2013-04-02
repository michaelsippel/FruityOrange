/**
 *  kernel/include/vfs.h
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
#ifndef _VFS_H
#define _VFS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stddef.h>

typedef struct vfs_inode {
  const char *name;
  uint8_t type;
  stat_t stat;
  
  void *base;
  size_t length;
  
  struct vfs_inode *parent;
} vfs_inode_t;

typedef struct vfs_dentry {
  const char *name;
  id_t id;
  
  struct vfs_inode *inode;
  struct vfs_dentry *parent;
} vfs_dentry_t;

typedef struct fd {
  vfs_inode_t *inode;
} fd_st_t;

void init_vfs(void);
vfs_inode_t *vfs_root(void);
vfs_inode_t *vfs_create_inode(const char *name, mode_t mode, vfs_inode_t *parent);
vfs_dentry_t *vfs_create_dentry(vfs_inode_t *inode);
int vfs_write(vfs_inode_t *inode, void *base, size_t bytes);
void* vfs_read(vfs_inode_t *inode, uintptr_t offset);
int vfs_access(vfs_inode_t *inode, mode_t modus);
void vfs_inode_list(vfs_inode_t *parent);

void vfs_init_syscalls(void);
void syscall_open(uint32_t *ebx, uint32_t *ecx, uint32_t *edx);
void syscall_close(uint32_t *ebx, uint32_t *ecx, uint32_t *edx);
void syscall_read(uint32_t *ebx, uint32_t *ecx, uint32_t *edx);
void syscall_write(uint32_t *ebx, uint32_t *ecx, uint32_t *edx);

vfs_inode_t *vfs_path_lookup(const char *path);

#endif
