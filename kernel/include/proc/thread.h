/**
 *  kernel/include/proc/thread.h
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
#ifndef _THREAD_H
#define _THREAD_H

#include <sys/types.h>
#include <stdint.h>

#include <proc/proc.h>
#include <proc/task.h>

typedef struct thread {
  tid_t tid;
  proc_t *proc;
  task_t *parent;
} thread_t;

thread_t *create_thread(task_t *task, void *entry);
int exit_thread(thread_t *thr, int status);
int kill_thread(thread_t *thr, int status);

#endif
