/**
 *  include/string.h
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
#ifndef _STRING_H
#define _STRING_H

#include <stdint.h>
#include <stddef.h>

size_t strlen(char *s);
void cut(char *s,int stelle,char *os1,char *os2);
void insert(char *s,int stelle,char *is);
void replace(char *s,int stelle, char *rs);
void memset(void *addr, int zeich, size_t n);
void* memcpy(void *ziel, const void *quelle, size_t n);
void strcpy(char *ziel, const char *quelle);
char* strcat(char *dest, const char *src);
int strcmp(char *str1, char *str2);
int vsprintf(char *buffer, const char *fmt, va_list args);
int sprintf(char *buffer, const char *format, ...);

#endif
