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
static size_t stack_size = 0x1000;

proc_t *first_proc = NULL;

proc_t *create_proc(void *entry, char *name, dpl_t dpl) {
  uint8_t *kern_stack = vmm_alloc();
//   uint8_t *user_stack;
//   if(dpl == 0) 	user_stack = kern_stack;
//   else		user_stack = vmm_alloc();
  
  cpu_state_t *proc_cpu_state = (void*) (kern_stack + stack_size - sizeof(cpu_state_t));
  vmm_map_page(current_context, kern_stack, kern_stack);
  *proc_cpu_state = (cpu_state_t) {
    .eax = 0, .ebx = 0, .ecx = 0, .edx = 0,
    .esi = 0, .edi = 0, .ebp = 0,
    
    .esp = (uint32_t) kern_stack + stack_size,
    .eip = (uint32_t) entry,
    
    .cs = 0x8,
    .eflags = 0x202,
  };
  
  proc_t *proc = vmm_alloc();
  
//   strcpy(proc->name, name);
  proc->pid = proc_count++;
//   proc->uid = 0;
//   proc->ticks = 3;
  proc->cpu = proc_cpu_state;
//   proc->context = vmm_create_context();
//   proc->used_mem_pages = 2;
  
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
