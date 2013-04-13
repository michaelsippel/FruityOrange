/**
 *  kernel/interrupt/handler/syscall.c
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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdint.h>
#include <stddef.h>

#include <cpu.h>
#include <driver/console.h>
#include <mm.h>
#include <interrupt.h>
#include <syscall.h>

static syscall_t **syscall_table = NULL;

void init_syscalltable(void) {
  syscall_table = malloc(sizeof(syscall_t*));
}

void setup_syscall(uint32_t id, const char *name, SYSCALL_HANDLER) {
  if(syscall_table[id] != NULL) {
    printf("Syscall %d is already occupied\n", id);
  } else {
    syscall_t *syscall = malloc(sizeof(syscall_t));
    syscall->name = name;
    syscall->id = id;
    syscall->handler = handler;
    
    syscall_table[id] = syscall;
  }
}

cpu_state_t *handle_syscall(cpu_state_t *cpu) {
  new_cpu = cpu;
  if(syscall_table[cpu->eax] == NULL) {
    printf("Undefined Syscall on 0x%x\n", cpu->eax);
  } else {
    syscall_table[cpu->eax]->handler(&new_cpu->ebx, &new_cpu->ecx, &new_cpu->edx);
  }
  
  return new_cpu;
}

