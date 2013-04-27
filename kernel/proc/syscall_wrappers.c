/**
 *  kernel/proc/syscall_wrappers.c
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
#include <stdint.h>
#include <string.h>
#include <sys/syscalls.h>

#include <driver/pit.h>
#include <proc/scheduler.h>
#include <proc/proc.h>
#include <syscall.h>
#include <mm.h>
#include <vfs.h>

void scheduler_init_syscalls(void) {
  setup_syscall(SYSCALL_EXIT, "exit", &syscall_exit);
  setup_syscall(SYSCALL_USLEEP, "usleep", &syscall_usleep);
  setup_syscall(SYSCALL_FORK, "fork", &syscall_fork);
  setup_syscall(SYSCALL_WAITPID, "waitpid", &syscall_waitpid);
  setup_syscall(SYSCALL_EXEC, "exec", &syscall_exec);
}

void syscall_exit(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  proc_exit(current_proc, *ebx);
}

void syscall_usleep(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  proc_t *proc = current_proc;
  proc->ticks_util_wake = *ebx / (1000000 / PIT_FREQ);
  proc_sleep(proc);
}

void syscall_fork(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  proc_t *child = proc_fork(current_proc);
  child->cpu->ebx = 0;
  *ebx = child->ppid;
}

void syscall_waitpid(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  proc_waitpid(current_proc, *ebx);
}

void syscall_exec(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  const char *path = *ebx;
  
  printf("exec: %s\n", path);
  vfs_inode_t *file = vfs_path_lookup(path);
  if(file == NULL) {
    printf("File not found!\n");
  }
}

