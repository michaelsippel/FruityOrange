/**
 *  kernel/include/driver/kbc.h
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
#ifndef _KBC_H
#define _KBC_H

#include <stdint.h>

#define KBC_PORT_KBDCOMMAND   0x60 /* command for Keyboard              */
#define KBC_PORT_KBCDATA      0x60 /* read & write data-buffer          */
#define KBC_PORT_KBCCOMMAND   0x64 /* command for KBC                   */
#define KBC_PORT_KBCREGISTER  0x64 /* read register                     */

#define KBC_COMM_P1_PORT   0xC0
#define KBC_COMM_READ_OPP  0xD0
#define KBC_COMM_WRITE_OPP 0xD1
#define KBC_COMM_READ_CCB  0x20
#define KBC_COMM_WRITE_CCB 0x60

#define KBC_COMMAND_OK 0xFA

void send_kbc_command(uint8_t port,uint8_t command);

#endif
