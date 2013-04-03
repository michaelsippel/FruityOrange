/**
 *  lib/user/unistd/file.c
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
#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>
#include <unistd.h>

fd_t open(const char *path, int oflags, mode_t mode) {
  int ret;
  asm volatile("int $0x30" : "=b" (ret) : "a" (SYSCALL_OPEN), "b" (path), "c" (oflags), "d" (mode));
  return ret;
}

void close(fd_t fd) {
  asm volatile("int $0x30" : : "a" (SYSCALL_CLOSE), "b" (fd));
}

int write(fd_t fd, const void *buf, size_t len) {
  int ret;
  asm volatile("int $0x30" : "=b" (ret) : "a" (SYSCALL_WRITE), "b" (fd), "c" (buf), "d" (len));
  return ret;
}

int read(fd_t fd, void *buf, size_t len) {
  int ret;
  asm volatile("int $0x30" : "=b" (ret) : "a" (SYSCALL_READ), "b" (fd), "c" (buf), "d" (len));
  return ret;
}

int lseek(fd_t fd, int off, int whence) {
  int ret;
  asm volatile("int $0x30" : "=b" (ret) : "a" (SYSCALL_SEEK), "b" (fd), "c" (off), "d" (whence));
  return ret;
}

int chdir(const char *path) {
  int ret;
  asm volatile("int $0x30" : "=c" (ret) : "a" (SYSCALL_CHDIR), "b" (path));
  return ret;
}
