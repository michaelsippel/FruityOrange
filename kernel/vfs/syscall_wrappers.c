/**
 *  kernel/vfs/syscall_wrappers.c
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
#include <sys/syscalls.h>
#include <unistd.h>
#include <string.h>

#include <proc/scheduler.h>
#include <vfs.h>
#include <syscall.h>

void vfs_init_syscalls(void) {
  setup_syscall(SYSCALL_OPEN, "open", &syscall_open);
  setup_syscall(SYSCALL_CLOSE, "close", &syscall_close);
  setup_syscall(SYSCALL_READ, "read", &syscall_read);
  setup_syscall(SYSCALL_WRITE, "write", &syscall_write);
  setup_syscall(SYSCALL_SEEK, "seek", &syscall_seek);
  setup_syscall(SYSCALL_CHDIR, "chdir", &syscall_chdir);
  setup_syscall(SYSCALL_GETCWD, "getcwd", &syscall_getcwd);
  setup_syscall(SYSCALL_READDIR, "readdir", &syscall_readdir);
}

void syscall_open(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  const char *path = (char*) *ebx;
  int oflags = *ecx;
  mode_t mode = *edx;
  
  fd_t fd = proc_get_unused_fd(current_proc);
  vfs_inode_t *inode = vfs_path_lookup(path);
  
  if(inode == NULL) { 
    if(oflags & O_CREAT) {// create inode
      // TODO
    } else {
      *ebx = -1;
      return;
    }
  } else {
    if(oflags & O_EXCL) {
      *ebx = -1;
      return;
    }
  }
  
  if(oflags & O_TRUNC) {
    memclr(inode->base, inode->length);
  }
  
  current_proc->fd[fd].inode = inode;
  current_proc->fd[fd].flags = oflags;
  current_proc->fd[fd].mode = mode;
  current_proc->fd[fd].pos = 0;
  
  *ebx = fd;
}

void syscall_close(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  fd_t fd = *ebx;
  memclr(&current_proc->fd[fd], sizeof(fd_st_t));
}

void syscall_read(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  fd_t fd = *ebx;
  void *buf = (void*) *ecx;
  size_t len = *edx;
  
  if(current_proc->fd[fd].flags & O_RDONLY ||
     current_proc->fd[fd].flags & O_RDWR) 
  {
    vfs_inode_t *inode = current_proc->fd[fd].inode;
    void *read = vfs_read(inode, current_proc->fd[fd].pos);
    if(read != NULL) {
      memcpy((void*)buf, read, len);
      current_proc->fd[fd].pos += len;
      *ebx = len;
    } else {
      *ebx = -1;
    }
  } else {
    *ebx = -1;
  }
}

void syscall_readdir(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  static int pos = 0;
  vfs_inode_t *parent;
  fd_t fd = *ebx;
  
  parent = current_proc->fd[fd].inode;  
  dirent_t *dentry = vmm_automap_user_page(current_context, pmm_alloc());//malloc(sizeof(dirent_t));  
  
  vfs_dentry_t *entries = vfs_read(parent, 0);  
  int num = parent->length / sizeof(vfs_dentry_t);  
  
  if(pos < num) {
    vfs_inode_t *ino = entries[pos++].inode;
    
    char *name = vmm_automap_user_page(current_context, pmm_alloc());//malloc(strlen(ino->name));
    strcpy(name, ino->name);
    dentry->name = name;
    dentry->id = ino->stat.id;
    *ebx = dentry;
  } else {
    pos = 0;
    *ebx = NULL;
  }
}

void syscall_write(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  fd_t fd = *ebx;
  const void *buf = (const void*) *ecx;
  size_t len = *edx;
  
  if(current_proc->fd[fd].flags & O_WRONLY ||
     current_proc->fd[fd].flags & O_RDWR) 
  {
    if(! current_proc->fd[fd].flags & O_APPEND) {
      current_proc->fd[fd].pos = 0;
      current_proc->fd[fd].flags |= O_APPEND;
    }
    vfs_inode_t *inode = current_proc->fd[fd].inode;
    *ebx = vfs_write(inode, current_proc->fd[fd].pos, buf, len);
    if(((int)*ebx) > 0) {
      current_proc->fd[fd].pos += len;
    }
  } else {
    *ebx = -1;
  }
}

void syscall_seek(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  fd_t fd = *ebx;
  int off = *ecx;
  int whence = *edx;
  
  current_proc->fd[fd].flags |= O_APPEND;
  switch(whence) {
    case SEEK_SET: // absolute
      current_proc->fd[fd].pos = off;
      break;
    case SEEK_CUR: // relative from current position
      current_proc->fd[fd].pos += off;
      break;
    case SEEK_END: // relative from end
      current_proc->fd[fd].pos = current_proc->fd[fd].inode->length - off;
      break;
    default: // ???
      *ebx = -1;
      return;
  }
  
  *ebx = current_proc->fd[fd].pos;
}

void syscall_chdir(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  const char *path = (const char*) *ebx;
  
  vfs_inode_t *inode = vfs_path_lookup(path);
  if(inode != NULL) {
    if(S_ISDIR(inode->stat)) {
      current_proc->work_dir = inode;
      *ecx = 0;
    } else {
      *ecx = -1;
    }
  } else {
    *ecx = -2;
  }
}

void syscall_getcwd(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  char *buf = (char*) *ebx;
  size_t len = *ecx;
  
  vfs_generate_path(buf, len, current_proc->work_dir);
}

