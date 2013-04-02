/**
 *  kernel/proc/syscall_wrappers.c
 *
 *  (C) Copyright 2012 Michael Sippel
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
#include <sys/syscalls.h>

#include <driver/pit.h>
#include <proc/scheduler.h>
#include <proc/proc.h>
#include <syscall.h>

void scheduler_init_syscalls(void) {
  setup_syscall(SYSCALL_EXIT, "exit", &syscall_exit);
  setup_syscall(SYSCALL_USLEEP, "usleep", &syscall_usleep);
}

void syscall_exit(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  proc_exit(current_proc, *ebx);
}

void syscall_usleep(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  proc_t *proc = current_proc;
  proc->ticks_util_wake = *ebx / (1000000 / PIT_FREQ);
  proc_sleep(proc);
}
