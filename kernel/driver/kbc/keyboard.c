/**
 *  kernel/driver/kbc/keyboard.c
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
#include <stddef.h>
#include <stdbool.h>

#include <portio.h>
#include <interrupt.h>
#include <syscall.h>

#include <driver/console.h>
#include <driver/kbc.h>
#include <driver/keyboard.h>

static volatile char buffer = 0;
static int modus;

void init_keyboard(void) {
  set_irq_handler(0x1, keyboard_irq_handler);
  
  while(! (inb(KBC_PORT_KBCREGISTER) & 0x4) );
  while (inb(KBC_PORT_KBCREGISTER) & 0x1) {
    inb(KBC_PORT_KBCDATA);
  }
  
  send_kbd_command(KBD_COMM_LEDS);
  outb(KBC_PORT_KBCDATA, 0);
  
  send_kbd_command(KBD_COMM_WDHR);
  outb(KBC_PORT_KBCDATA, 0);
  
  send_kbd_command(KBD_COMM_ACTV);
  
  setup_syscall(SYSCALL_GETC, "getch", &getc_syscall_wrapper);
  setup_syscall(SYSCALL_GETS, "gets",  &gets_syscall_wrapper);
}

void keyboard_irq_handler(void) {
    uint8_t scancode;
    uint8_t keycode = 0;
    bool break_code = 1;
    
    static int e0_code = 0;
    static int e1_code = 0;
    static uint16_t e1_prev = 0;
    
    scancode = inb(KBC_PORT_KBCDATA);
    
    // Breakcode?
    if((scancode & 0x80) &&
       (e1_code || (scancode != 0xE1)) &&
       (e0_code || (scancode != 0xE0)))
    {
	break_code = 0;
	scancode &= ~0x80;
	keycode = translate_scancode(SNC_TYPE_STD, scancode);
	send_key_event(keycode, break_code);
	return;
    }
    
    if(e0_code) {
	e0_code = 1;
	// catch fakeshift
	if((scancode == 0x2A) || (scancode == 0x36)){
	    return;
	}
	keycode = translate_scancode(SNC_TYPE_E0, scancode);
    } else if(e1_code == 2) {
	// full E1-scancode
	// shift twice scancode into higher byte
	e1_prev |= ((uint16_t) scancode << 8);
	keycode = translate_scancode(SNC_TYPE_E1, e1_prev);
	e1_code = 0;
    } else if(e1_code == 1) {
	// first byte for E1-scancode
	e1_prev = scancode;
	e1_code++;
    } else if(scancode == 0xE0) {
	// E0-Code
	e0_code = 0;
    } else if(scancode == 0xE1) {
	// E1-Code
	e1_code = 1;
    } else {
	// standard scancode
	keycode = translate_scancode(SNC_TYPE_STD, scancode);
    }
    send_key_event(keycode, break_code);
}

void send_key_event(uint8_t data, bool breaked) {
    if(! breaked) {
      switch(data) {
	case 53:
	case 67:
	case 56:
	  modus = 0;
      }
    } else {
      switch(data) {
	case 65:
	  puts("\r");break;
	case 40:
	  modus = (modus == 1) ? 0 : 1;break;
	case 53:
	case 67:
	  modus = 1;break;
	case 56:
	  modus = 2;break;
	default: 
	  buffer = data;
	  getc_syscall_end();
      }
    }
}

char read_kbd_buffer(void) {
  return buffer;
}

char getch(void) {
  buffer = 0;
  while(! buffer);
  return translate_keycode(buffer, modus);
}

int gets(char *buf) {
  int i = 0;
  while(1) {
    buffer = 0;
    while(! buffer);
    if(buffer == 66) {
        *buf = '\0';
	puts("\n");
	return i;
    } else {
      buffer = translate_keycode(buffer, modus);
      *buf++ = buffer;
      printf("%c", buffer);
      i++;
    }
  }
}