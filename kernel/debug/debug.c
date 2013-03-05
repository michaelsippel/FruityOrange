/**
 *  kernel/debug.c
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
#include <stdarg.h>
#include <string.h>

#include <driver/console.h>
#include <debug/debug.h>

void debug(int print, const char *fmt, ...) {
  #if DEBUG_PRINT
  if(print) {
    char color = getColor();
    setForegroundColor(0x8);
    
    va_list args;
    char t[1024];
    char *buffer = (char*) t;
    
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    
    puts(buffer);
    
    setColor(color);
  }
  #endif
}

void kinip(char *str) {
  #if INI_PRINT == 1
  printf("[kernelinit] %s", str);
  #endif
}

void dinip(char *str) {
  #if INI_PRINT == 1
  printf("[driverinit] %s", str);
  #endif
}

void endini(void) {
  #if INI_PRINT == 1
  printf("(done)\n");
  #endif
}
