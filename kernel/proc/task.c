/**
 *  kernel/proc/task.c
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
#include <sys/types.h>
#include <stdint.h>

#include <cpu.h>
#include <mm.h>
#include <proc/scheduler.h>
#include <proc/task.h>

static pid_t proc_count = 0;
static size_t stack_size = 0x1000;

proc_t *first_proc = NULL;

proc_t *create_proc(void *entry, char *name, uint8_t dpl) {
  uint8_t *kern_stack = vmm_alloc();
  uint8_t *user_stack;
  if(dpl == 0) 	user_stack = kern_stack;
  else		user_stack = vmm_alloc();
  
  cpu_state_t *proc_cpu_state = (void*) (kern_stack + stack_size - sizeof(cpu_state_t));
  *proc_cpu_state = (cpu_state_t) {
    .eax = 0, .ebx = 0, .ecx = 0, .edx = 0,
    .esi = 0, .edi = 0, .ebp = 0,
    
    .esp = (uint32_t) user_stack + stack_size,
    .eip = (uint32_t) entry,
    
    .cs = 0x08 | (dpl&2) | (dpl&1)*0x10,
    .ss = 0x10 | (dpl&2) | (dpl&1)*0x10,
    
    .eflags = 0x202,
  };
  
  proc_t *proc = vmm_alloc();//malloc(sizeof(proc_t));
  
  strcpy(proc->name, name, sizeof(name));
  proc->pid = proc_count++;
  proc->uid = 0;
  proc->ticks = 3;
  proc->cpu = proc_cpu_state;
  if(dpl == 0) proc->context = NULL;
  else {
    proc->context = vmm_create_context();
    proc->context->flags = VMM_USER_FLAGS;
  }
  proc->used_mem_pages = 2;
  
  if(first_proc == NULL) {
    proc->next = proc;
    proc->prev = proc;
    first_proc = proc;
  }
  
  return proc;
}
