/**
 *  kernel/driver/kbc/syscall_wrapper.c
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
#include <sys/syscalls.h>
#include <stdint.h>
#include <stdbool.h>

#include <driver/keyboard.h>
#include <proc/proc.h>

#define NONE 0x0
#define GETC 0x1
#define GETS 0x2

static int use = NONE;
static proc_t *proc = NULL;

void getc_syscall_wrapper(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  if(use == NONE) {
    use = GETC;
    proc = get_current_proc();
    proc_sleep(proc);
  }
}

void gets_syscall_wrapper(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  if(use == NONE) {
    use = GETS;
    proc = get_current_proc();
    proc_sleep(proc);
  }
}

void syscall_step(void) {
  char buf = read_kbd_buffer();
  char mod = read_kbd_modus();
  
  static int i = 0;
  
  switch(use) {
    case GETC:
      proc->cpu->ebx = buf;
      proc->cpu->ecx = mod;
      proc_wake(proc);
      
      proc = NULL;
      use = NONE;
      break;
    case GETS:
      if(buf == 66) {
	use = NONE;
	proc_wake(proc);
	proc = NULL;
      } else {
	char *s = proc->cpu->ebx;
	s[i++] = buf;
	printf("%c %d\n", translate_keycode(buf, mod), buf);
      }
      break;
      
    default: break;
  }
}
