/**
 *  kernel/include/syscall.h
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
#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <stdint.h>
#include <stddef.h>

#define SYSCALL_HANDLER void (*handler)(uint32_t *ebx, uint32_t *ecx, uint32_t *edx)

typedef struct syscall {
  const char *name;
  uint32_t id; // %eax
  SYSCALL_HANDLER;
} syscall_t;

void init_syscalltable(void);
void setup_syscall(uint32_t id, const char *name, SYSCALL_HANDLER);

#endif
