/**
 *  kernel/mm/pagefault.c
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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <mm.h>
#include <interrupt.h>
#include <proc/scheduler.h>

#define PRESENT  0x01
#define WRITE    0x02
#define USER     0x04
#define RESERVED 0x08
#define CODE     0x10

int pagefault(uint32_t cr0, uint32_t cr2, uint32_t cr3) {
  int error = new_cpu->error_code;
  printf("pagefault while %s a %s page\nat address 0x%x in %sspace.\n", 
    (error & WRITE) ? "writing to" : "reading from",
    (error & PRESENT) ? "present" : "non-present", 
    cr2,
    (error & USER) ? "user" : "kernel"
  );
  
  printf("EIP: 0x%x\n", new_cpu->eip);
  
  printf("kill process %s with pid %d...\n",current_proc->name,current_proc->pid);
  proc_exit(current_proc, 1);
  return 1;
}

