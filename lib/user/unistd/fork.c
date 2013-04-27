/**
 *  lib/user/unistd/fork.c
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

pid_t fork(void) {
  pid_t pid;
  asm volatile("int $0x30" : "=b" (pid) : "a" (SYSCALL_FORK));
  return pid;
}

void waitpid(pid_t pid) {
  asm volatile("int $0x30" : : "a" (SYSCALL_WAITPID), "b" (pid));
}

void exec(const char *path) {
  asm volatile("int $0x30" : : "a" (SYSCALL_EXEC), "b" (path));
}

