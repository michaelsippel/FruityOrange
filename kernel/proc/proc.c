/**
 *  kernel/proc/proc.c
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
#define _PROC_C

#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdint.h>

#include <cpu.h>
#include <mm.h>
#include <vfs.h>
#include <init/gdt.h>
#include <debug/debug.h>
#include <proc/scheduler.h>
#include <proc/proc.h>

#define PROC_DEBUG 0

static pid_t proc_count = 0;
static size_t kernel_stack_size = 0x1000;
static size_t user_stack_size   = 0x1000;

proc_t *first_proc = NULL;

proc_t *create_proc(void *entry, const char *name, vmm_context_t *context, dpl_t dpl) {
  // Process structure
  proc_t *proc = malloc(sizeof(proc_t));
  proc->name = name;
  proc->context = context;
  proc->used_mem_pages = 0;
  
  proc->waitpid = 0;
  proc->child_count = 0;
  proc->pid = get_pid();
  proc->ppid = 0;
  proc->uid = 0;
  proc->dpl = dpl;
  
  proc->ticks = 3;
  proc->ticks_util_wake = -1;
  proc->status = ACTIVE;
  
  proc->num_fd = 1;
  proc->fd = calloc(proc->num_fd, sizeof(fd_st_t));
  //#define MODE S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH
  //proc->fd[0].inode = vfs_create_inode("stdout", MODE, NULL);
  //proc->fd[1].inode = vfs_create_inode("stdin",  MODE, NULL);
  //proc->fd[2].inode = vfs_create_inode("stderr", MODE, NULL);
  
  proc->work_dir = vfs_root();
  
  // Stack
  uintptr_t kernel_stack = (uintptr_t) malloc(kernel_stack_size);
  
  cpu_state_t *proc_cpu_state = (void*) (kernel_stack + kernel_stack_size - sizeof(cpu_state_t));
  *proc_cpu_state = (cpu_state_t) {
    .eax = 0, .ebx = 0, .ecx = 0, .edx = 0,
    .esi = 0, .edi = 0, .ebp = 0,
    
    .eip = (uint32_t) entry,
    
    .eflags = 0x202,
  };
  proc->cpu = proc_cpu_state;
  proc->kernel_stack = kernel_stack;
  
  if(dpl) { // Usermode
    uintptr_t user_stack_phys = (uintptr_t) pmm_alloc();
    uintptr_t user_stack = (uintptr_t) vmm_automap_user_page(context, user_stack_phys);
    
    proc->user_stack_phys = user_stack_phys;
    proc->user_stack = user_stack;
    
    proc_cpu_state->esp = user_stack + user_stack_size;
    proc_cpu_state->cs = _USER_CS;
    proc_cpu_state->ss = _USER_SS;
  } else { // Kernelmode
    proc_cpu_state->cs = _KERNEL_CS;
    proc_cpu_state->ds = _KERNEL_DS;
    proc_cpu_state->es = _KERNEL_DS;
    proc_cpu_state->fs = _KERNEL_DS;
    proc_cpu_state->gs = _KERNEL_DS;
  }
  
  proc->parent = NULL;
  if(proc_count == 1) {
    proc->next = proc;
    proc->prev = proc;
  } else {
    proc->next = first_proc;
    proc->prev = first_proc->prev;
    first_proc->prev->next = proc;
    first_proc->prev = proc;
  }
  first_proc = proc;
  
  debug(PROC_DEBUG, "create_proc(): created process \"%s\" with pid %d.\n", proc->name, proc->pid);
  
  return proc;
}

fd_t proc_get_unused_fd(proc_t *proc) {
  proc->fd = realloc(proc->fd, proc->num_fd++);
  
  return proc->num_fd-1;
}

pid_t get_pid(void) {
  return proc_count++;
}

proc_t *proc_fork(proc_t *parent) {
  vmm_context_t *context = vmm_fork(parent->context);
  proc_t *child = create_proc(0, parent->name, context, parent->dpl);
  memcpy(child->kernel_stack, parent->kernel_stack, PAGE_SIZE);  
  
  if(parent->dpl) {
    child->cpu->esp = child->user_stack + (parent->cpu->esp - parent->user_stack);
    void *cur_stack = vmm_automap_kernel_page(current_context, parent->user_stack_phys);
    void *new_stack = vmm_automap_kernel_page(current_context, child->user_stack_phys);
    
    memcpy((uintptr_t)new_stack, (uintptr_t)cur_stack, PAGE_SIZE);
    
    vmm_unmap_page(current_context, (uintptr_t)cur_stack);
    vmm_unmap_page(current_context, (uintptr_t)new_stack);
  }
  
  child->parent = parent;
  child->ppid = ++parent->child_count;
  child->status = ACTIVE;
  child->used_mem_pages = parent->used_mem_pages;
  child->work_dir = parent->work_dir;
  
  debug(PROC_DEBUG, "proc_fork(): forked pid %d from pid %d\n", child->pid, parent->pid);  
  
  return child;
}

void proc_waitpid(proc_t *proc, pid_t pid) {
  proc_sleep(proc);
  proc->waitpid = pid;
}

int proc_sleep(proc_t *proc) {
  if(proc->status != SLEEP) {
    proc->status = SLEEP;
    
    if(proc == current_proc) {
      schedule();
    }
    
  } else {
    debug(PROC_DEBUG, "proc_sleep(): process %d is already asleep!\n", proc->pid);
    return -1;
  }
  
  return 0;
}

int proc_wake(proc_t *proc) {
  if(proc->status != ACTIVE) {
    proc->status = ACTIVE;
  } else {
    debug(PROC_DEBUG, "proc_wake(): process %d is already wake!\n", proc->pid);
    return -1;
  }
  
  return 0;
}

int proc_exit(proc_t *proc, int status) {
  debug(PROC_DEBUG, "proc_exit(): exit process %d with status %d.\n",proc->pid, status);
  proc->status = ZOMBIE;
  
  if(proc->parent) {
    if(proc->parent->waitpid == proc->ppid) {
      proc->parent->waitpid = 0;
      proc_wake(proc->parent);
    }
  }
  
  // TODO
  proc_kill(proc);
  
  return status;
}

int proc_kill(proc_t *proc) {
  debug(PROC_DEBUG, "proc_kill(): kill process %d.\n",proc->pid);
  if(proc == current_proc) {
    schedule();
  }
  
  // remove from list
  proc->prev->next = proc->next;
  proc->next->prev = proc->prev;
  
  // free data
  free(proc->kernel_stack);
  free(proc);
  
  return 0;
}

