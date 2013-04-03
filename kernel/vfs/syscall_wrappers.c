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
}

void syscall_open(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  const char *path = (char*) *ebx;
  int oflags = *ecx;
  mode_t mode = *edx;
  
  fd_t fd = proc_get_unused_fd(current_proc);
  vfs_inode_t *inode = vfs_path_lookup(path);
  
  current_proc->fd[fd].inode = inode;
  current_proc->fd[fd].flags = oflags;
  current_proc->fd[fd].mode = mode;
  current_proc->fd[fd].pos = 0;
  
  *ebx = fd;
}

void syscall_close(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  
}

void syscall_read(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  fd_t fd = *ebx;
  void *buf = (void*) *ecx;
  size_t len = *edx;
  
  vfs_inode_t *inode = current_proc->fd[fd].inode;
  memcpy((void*)buf, vfs_read(inode, current_proc->fd[fd].pos), len);
  current_proc->fd[fd].pos += len;
}

void syscall_write(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  fd_t fd = *ebx;
  const void *buf = (const void*) *ecx;
  size_t len = *edx;
  
  vfs_inode_t *inode = current_proc->fd[fd].inode;
  *ebx = vfs_write(inode, current_proc->fd[fd].pos, buf, len);
  current_proc->fd[fd].pos += len;
}

void syscall_seek(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  fd_t fd = *ebx;
  int off = *ecx;
  int whence = *edx;
  
  switch(whence) {
    case SEEK_SET: // absolute
      current_proc->fd[fd].pos = off;
      break;
    case SEEK_CUR: // relative from begin
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
