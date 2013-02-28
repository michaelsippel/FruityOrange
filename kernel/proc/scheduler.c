/**
 *  kernel/proc/scheduler.c
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

#include <init/gdt.h>
#include <cpu.h>
#include <mm.h>
#include <interrupt.h>

#include <proc/scheduler.h>
#include <proc/proc.h>

static proc_t *current_proc = NULL;

void init_scheduler(void) {
  set_irq_handler(0x0, schedule);
}

void activate_proc(proc_t *proc) {
  set_cpu_state(proc->cpu);
  tss[1] = (uint32_t) (proc->cpu + 1);
  vmm_activate_context(proc->context);
}

void schedule(void) {
#define LOOK_FOR_ACTIVE_PROC() \
  do { \
    current_proc = current_proc->next; \
  } while(current_proc->status != ACTIVE);
  
  if(current_proc != NULL) {
    current_proc->cpu = get_cpu_state();
    LOOK_FOR_ACTIVE_PROC();
    activate_proc(current_proc);
  } else {
    if(first_proc != NULL) {
      current_proc = first_proc;
      LOOK_FOR_ACTIVE_PROC();
      activate_proc(current_proc);
    }
  }
  common_eoi(0x20);
}
