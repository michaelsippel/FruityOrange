/**
 *  kernel/include/driver/keyboard.h
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
#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

#include <driver/kbc.h>

#define KBD_COMM_LEDS 0xED /* control LEDs          */
#define KBD_COMM_ECHO 0xEE /* Echo                  */
#define KBD_COMM_WDHR 0xF3 /* set repetition rate   */
#define KBD_COMM_ACTV 0xF4 /* activate keyboard     */
#define KBD_COMM_DAKV 0xF5 /* deactivate keyboard   */
#define KBD_COMM_STDV 0xF6 /* Reset values          */
#define KBD_COMM_KRST 0xFF /* reset keyboard        */

#define SNC_TYPE_STD 0
#define SNC_TYPE_E0 1
#define SNC_TYPE_E1 2

#define KBD_ASSIGNMENT_DE_DE 0
#define KBD_ASSIGNMENT_EN_US 1

void init_keyboard(void);
void keyboard_irq_handler(void);
uint8_t translate_scancode(int type, uint16_t scancode);
uint8_t translate_keycode(uint8_t keycode, int modus);
void send_key_event(uint8_t data, bool breaked);
void set_kbd_assignment(int id);

char getch(void);
int  gets(char *buf);

char read_kbd_buffer(void);
int  read_kbd_modus(void);

void getc_syscall_wrapper(uint32_t *ebx, uint32_t *ecx, uint32_t *edx);
void gets_syscall_wrapper(uint32_t *ebx, uint32_t *ecx, uint32_t *edx);
void syscall_step(void);

static inline void send_kbd_command(uint8_t command) {
   send_kbc_command(KBC_PORT_KBDCOMMAND,command);
}

#endif
