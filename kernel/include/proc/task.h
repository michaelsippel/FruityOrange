/**
 *  kernel/include/proc/task.h
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
#ifndef _TASK_H
#define _TASK_H

#include <sys/types.h>
#include <stdint.h>

#include <cpu.h>
#include <iostream.h>
#include <mm.h>

struct proc;
struct task;
struct thread;

typedef struct proc {
  char *name;
  pid_t pid;
  uid_t uid;
  int ticks;
  cpu_state_t *cpu;
  vmm_context_t *context;
  size_t used_mem_pages;
  
  struct proc *next;
  struct proc *prev;
} proc_t;

typedef struct task {
  proc_t *proc;
  iostream_t *iostream;
  
  uint32_t num_threads;
  struct thread **threads;
} task_t;

typedef struct thread {
  tid_t tid;
  proc_t *proc;
  task_t *parent;
} thread_t;

proc_t *create_proc(void *entry, char *name, uint8_t dpl);
task_t *create_task(void *entry, char *name);
thread_t *create_thread(task_t *task, void *entry);

#endif
