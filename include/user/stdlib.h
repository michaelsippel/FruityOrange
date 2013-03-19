/**
 *  include/user/stdlib.h
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
#ifndef _USER_STDLIB_H
#define _USER_STDLIB_H

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

void exit(int status);

void *alloc_pages(size_t num);
void free_pages(uintptr_t ptr, size_t num);
void *malloc(size_t bytes);
void *calloc(size_t num, size_t size);
void *realloc(void *ptr, size_t bytes);
void free(void *ptr);

#endif
