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

#include <sys/types.h>
#include <string.h>
#include <stdint.h>

#include <cpu.h>
#include <mm.h>
#include <init/gdt.h>
#include <proc/scheduler.h>
#include <proc/proc.h>

static pid_t proc_count = 0;
static size_t kernel_stack_size = 0x1000;
static size_t user_stack_size   = 0x1000;

proc_t *first_proc = NULL;

proc_t *create_proc(void *entry, const char *name, vmm_context_t *context, dpl_t dpl) {
  // Process structure
  proc_t *proc = malloc(sizeof(proc_t));
  proc->name = name;
  proc->context = context;
  proc->pid = proc_count++;
  proc->uid = 0;
  proc->ticks = 3;
  proc->dpl = dpl;
  proc->used_mem_pages = 2;
  
  // Stack
  uintptr_t kernel_stack = malloc(kernel_stack_size);
  
  cpu_state_t *proc_cpu_state = (void*) (kernel_stack + kernel_stack_size - sizeof(cpu_state_t));
  *proc_cpu_state = (cpu_state_t) {
    .eax = 0, .ebx = 0, .ecx = 0, .edx = 0,
    .esi = 0, .edi = 0, .ebp = 0,
    
    .eip = (uint32_t) entry,
    
    .eflags = 0x202,
  };
  proc->cpu = proc_cpu_state;
  
  if(dpl) { // Usermode
    uintptr_t user_stack = malloc(user_stack_size);
    
    proc_cpu_state->esp = user_stack + user_stack_size;
    proc_cpu_state->cs = _USER_CS;
    proc_cpu_state->ss = _USER_SS;
//     proc_cpu_state->ds = _USER_DS;
//     proc_cpu_state->es = _USER_DS;
//     proc_cpu_state->fs = _USER_DS;
//     proc_cpu_state->gs = _USER_DS;
  } else { // Kernelmode
    proc_cpu_state->cs = _KERNEL_CS;
    proc_cpu_state->ds = _KERNEL_DS;
//     proc_cpu_state->es = _KERNEL_DS;
//     proc_cpu_state->fs = _KERNEL_DS;
//     proc_cpu_state->gs = _KERNEL_DS;
  }
  
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
  
  return proc;
}
