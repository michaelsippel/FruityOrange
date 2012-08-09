/**
 *  kernel/driver/kbc/kbc.c
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

#include <portio.h>
#include <driver/kbc.h>


void send_kbc_command(uint8_t port,uint8_t command){
  static int fz = 3; 
  uint8_t ret;
  
  while( inb(KBC_PORT_KBCREGISTER) & 0x2 );
  outb(port, command);
  while(inb(KBC_PORT_KBCREGISTER) & 0x1);
  ret = inb(KBC_PORT_KBCDATA);

  if(ret == KBC_COMMAND_OK || fz >= 3){
    fz = 3;
    return;
  }else{
    --fz;
    send_kbc_command(port, command);
  }
}
