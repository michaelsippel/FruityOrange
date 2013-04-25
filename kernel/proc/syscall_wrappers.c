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

void scheduler_init_syscalls(void) {
  setup_syscall(SYSCALL_EXIT, "exit", &syscall_exit);
  setup_syscall(SYSCALL_USLEEP, "usleep", &syscall_usleep);
  setup_syscall(SYSCALL_FORK, "fork", &syscall_fork);
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
  vmm_context_t *context = current_proc->context;//vmm_fork(current_proc->context);
  proc_t *new_proc = create_proc(0, current_proc->name, context, current_proc->dpl);
  memcpy(new_proc->kernel_stack, current_proc->kernel_stack, PAGE_SIZE);
  
  if(current_proc->dpl) {
    void *cur_stack = vmm_automap_kernel_page(current_context, current_proc->user_stack);
    void *new_stack = vmm_automap_kernel_page(current_context, new_proc->user_stack);
    
    memcpy(new_stack, cur_stack, PAGE_SIZE);
    
    vmm_unmap_page(current_context, cur_stack);
    vmm_unmap_page(current_context, new_stack);
  }
  
  new_proc->status = current_proc->status;
  new_proc->used_mem_pages = current_proc->used_mem_pages;  
  
  *ebx = 1;
  new_proc->cpu->ebx = 0;
}

