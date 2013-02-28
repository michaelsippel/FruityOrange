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

static bool syscall_used = FALSE;
static proc_t *proc = NULL;

void getc_syscall_wrapper(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  if(! syscall_used) {
    syscall_used = TRUE;
    proc = get_current_proc();
    proc_sleep(proc);
  }
}

void getc_syscall_end(void) {
  if(syscall_used) {
    char buf = read_kbd_buffer();
    printf("%c", translate_keycode(buf, read_kbd_modus()));
    proc->cpu->ebx = buf;
    proc_wake(proc);
    
    proc = NULL;
    syscall_used = FALSE;
  }
}

void gets_syscall_wrapper(uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  *ecx = gets(ebx);
}
