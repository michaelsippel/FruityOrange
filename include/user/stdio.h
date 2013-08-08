/**
 *  include/user/stdio.h
 *
 *  (C) Copyright 2013 Michael Sippel
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
#ifndef _STDIO_H
#define _STDIO_H

#include <sys/file.h>
#include <stddef.h>

inline int putchar(const char ch);
inline int puts(const char *s);
int printf(char *format, ...);

inline char getch(void);
inline char *gets(char *s);

FILE *fopen(const char path, char *modus);
int fclose(FILE *file);
size_t fread(void *buf, size_t size, size_t n, FILE *file);
size_t fwrite(void *buf, size_t size, size_t n, FILE *file);
void fseek(FILE *file, int off, int whence);
int ftell(FILE *file);

#endif

