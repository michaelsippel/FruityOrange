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
extern proc_t *first_proc;
extern uint32_t tss[TSS_SIZE];

void init_scheduler(void) {
  set_irq_handler(0x0, schedule);
}

void activate_proc(proc_t *proc) {
  cpu_state_t *cpu = get_cpu_state();
  cpu_state_t *new_cpu = proc->cpu;
  if(cpu != new_cpu) {
    set_cpu_state(new_cpu);
    tss[1] = (uint32_t) (new_cpu + 1);
    if(proc->context != NULL) {
      vmm_activate_context(proc->context);
    }
  }
}

void schedule(void) {
  if(current_proc != NULL) {
    current_proc->cpu = get_cpu_state();
    current_proc = current_proc->next;
    activate_proc(current_proc);
  } else {
    if(first_proc != NULL) {
      current_proc = first_proc;
      activate_proc(current_proc);
    }
  }
  common_eoi(0x20);
}
