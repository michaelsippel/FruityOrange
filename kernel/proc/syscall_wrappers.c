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
#include <driver/console.h>
#include <proc/scheduler.h>
#include <proc/proc.h>
#include <syscall.h>
#include <mm.h>
#include <vfs.h>
#include <init/gdt.h>

void scheduler_init_syscalls(void) {
  setup_syscall(SYSCALL_EXIT, "exit", &syscall_exit);
  setup_syscall(SYSCALL_USLEEP, "usleep", &syscall_usleep);
  setup_syscall(SYSCALL_FORK, "fork", &syscall_fork);
  setup_syscall(SYSCALL_WAITPID, "waitpid", &syscall_waitpid);
  setup_syscall(SYSCALL_EXEC, "exec", &syscall_exec);
  setup_syscall(SYSCALL_EXEC_EXTERN, "exec extern", &syscall_exec_extern);
  setup_syscall(SYSCALL_PUTENV, "putenv", &syscall_putenv);
  setup_syscall(SYSCALL_GETENV, "getenv", &syscall_getenv);
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
  const char *path = (const char*) *ebx;
  int argc = *ecx;
  char **argv = (char**) *edx;
  
  vfs_inode_t *file = vfs_path_lookup(path);
  if(file == NULL) {
    printf("File not found!\n");
  } else {
    loaded_elf_t *elf = load_elf32(file->base, current_context, file->name);    
    current_proc->name = elf->name;
    current_proc->context = elf->context;
    current_proc->used_mem_pages = 0;
    
    current_proc->ticks = 3;
    current_proc->ticks_util_wake = -1;
    current_proc->status = ACTIVE;
    
    *current_proc->cpu = (cpu_state_t) {
      .eax = (uint32_t)argc, .ebx = (uint32_t)argv, .ecx = 0, .edx = 0,
      .esi = 0, .edi = 0, .ebp = 0,
      
      .eip = (uint32_t) elf->entry,
      
      .eflags = 0x202,
    };
    current_proc->name = elf->name;
    current_proc->context = elf->context;
    current_proc->dpl = elf->dpl;
    
    if(elf->dpl) {
      void *stack = vmm_automap_kernel_page(current_context, current_proc->user_stack_phys);
      memclr(stack, PAGE_SIZE);
      vmm_unmap_page(current_context, (uintptr_t) stack);
      
      current_proc->cpu->esp = current_proc->user_stack + PAGE_SIZE;
      current_proc->cpu->cs = _USER_CS;
      current_proc->cpu->ss = _USER_SS;
    }
  }
}

void syscall_exec_extern(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  const char *path = (const char*) *ebx;
  int argc = *ecx;
  char **argv = (char**) *edx;
  
  vfs_inode_t *file = vfs_path_lookup(path);
  if(file == NULL) {
    printf("File not found! (%s)\n", path);
  } else {
    vmm_context_t *context = vmm_create_context();
    loaded_elf_t *elf = load_elf32(file->base, context, file->name);
    proc_t *new_p = run_elf32(elf);
    new_p->cpu->eax = (uint32_t) argc;
    new_p->cpu->ebx = (uint32_t) argv;
    new_p->parent = current_proc;
    new_p->ppid = ++current_proc->child_count;
    new_p->status = ACTIVE;
    
    *ebx = new_p->ppid;
  }
}

void syscall_putenv(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  char *name = *ebx;
  char *value = *ecx;
  
  env_t *env = malloc(sizeof(env_t));
  strcpy(env->name, name);
  strcpy(env->value, value);
  
  env->next = current_proc->environment;
  current_proc->environment = env;
}

void syscall_getenv(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  char *name = *ebx;
  char *buf = *ecx;  
  
  env_t *env = current_proc->environment;
  while(env != NULL) {
    if(strcmp(name, env->name)) {
      strcpy(buf, env->value);
      return;
    }
  }
}

