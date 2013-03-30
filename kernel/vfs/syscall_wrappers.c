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

#include <vfs.h>
#include <syscall.h>

void vfs_init_syscalls(void) {
  setup_syscall(SYSCALL_OPEN, "fopen", &syscall_open);
  setup_syscall(SYSCALL_CLOSE, "fclose", &syscall_close);
  setup_syscall(SYSCALL_READ, "fread", &syscall_read);
  setup_syscall(SYSCALL_WRITE, "fwrite", &syscall_write);
}

// TODO
void syscall_open(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  
}

void syscall_close(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  
}

void syscall_read(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  
}

void syscall_write(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  
}
