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
#include <stdarg.h>

size_t strlen(char *s);
void cut(char *s,int pos,char *os1,char *os2);
void insert(char *s,int pos,char *is);
void replace(char *s,int pos, char *rs);
void strcpy(char *dest, const char *src);
char *strtok(const char *string, char *delimiters);
char *strcat(char *dest, const char *src);
char *strchr(const char *s, char c);
int strcmp(const char *str1, char *str2);

void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *addr, int zeich, size_t n);
void *memclr(void *addr, size_t n);

int vsprintf(char *buffer, const char *fmt, va_list args);
int sprintf(char *buffer, const char *format, ...);

#endif
