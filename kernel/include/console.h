/**
 *  kernel/include/console.h
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
#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <stdint.h>

int putchar(char chr);
int puts(const char *str);
void scroll(void);
void setCursor(int x, int y);
void setColor(uint8_t ncolor);
void setForegroundColor(uint8_t fcolor);
void setBackgroundcolor(uint8_t bcolor);
void clearscreen(void);

#endif
