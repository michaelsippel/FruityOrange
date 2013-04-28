/**
 *  kernel/include/proc/proc.h
 *
 *  (C) Copyright 2012-2013 Michael Sippel
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
#ifndef _PROC_H
#define _PROC_H

#include <sys/file.h>
#include <sys/types.h>
#include <stdint.h>

#include <cpu.h>
#include <mm.h>
#include <vfs.h>

#define PROC_STATUS_ACTIVE  0x0
#define PROC_STATUS_SLEEP   0x1
#define PROC_STATUS_ZOMBIE  0x2

typedef enum proc_status {
  ACTIVE = PROC_STATUS_ACTIVE,
  SLEEP = PROC_STATUS_SLEEP,
  ZOMBIE = PROC_STATUS_ZOMBIE
} proc_status_t;

typedef struct proc {
  const char *name;
  pid_t pid;
  pid_t ppid;
  pid_t waitpid;
  pid_t child_count;
  
  uid_t uid;
  dpl_t dpl;
  
  uintptr_t kernel_stack;
  uintptr_t user_stack;
  uintptr_t user_stack_phys;
  cpu_state_t *cpu;
  vmm_context_t *context;
  unsigned int num_fd;
  fd_st_t *fd;
  vfs_inode_t *work_dir;
  
  size_t used_mem_pages;
  unsigned long ticks;
  unsigned long ticks_util_wake;
  proc_status_t status;
  
  struct proc *parent;  
  
  struct proc *next;
  struct proc *prev;
} proc_t;

typedef struct loaded_elf {
  void *entry;
  const char *name;
  vmm_context_t *context;
  dpl_t dpl;
} loaded_elf_t;

#ifndef _PROC_C
extern proc_t *first_proc;
#endif

loaded_elf_t *load_elf32(void *image, vmm_context_t *context, const char *name);
proc_t *run_elf32(loaded_elf_t *elf);
proc_t *create_proc(void *entry, const char *name, vmm_context_t *context, dpl_t dpl);
int proc_sleep(proc_t *proc);
int proc_wake(proc_t *proc);
int proc_exit(proc_t *proc, int status);
int proc_kill(proc_t *proc);
fd_t proc_get_unused_fd(proc_t *proc);
pid_t get_pid(void);

void syscall_exit(uint32_t *ebx, uint32_t *ecx, uint32_t *edx);
void syscall_usleep(uint32_t *ebx, uint32_t *ecx, uint32_t *edx);
void syscall_fork(uint32_t *ebx, uint32_t *ecx, uint32_t *edx);
void syscall_waitpid(uint32_t *ebx, uint32_t *ecx, uint32_t *edx);
void syscall_exec(uint32_t *ebx, uint32_t *ecx, uint32_t *edx);

#endif

