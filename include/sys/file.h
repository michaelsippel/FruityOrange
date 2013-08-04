/**
 *  include/sys/file.h
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
#ifndef _FILE_H
#define _FILE_H

#include <stdint.h>
#include <sys/stat.h>

#define EOF (-1)

typedef struct file {
  char *fpos;
  void *base;
  uint8_t handle;
  int8_t flags;
  int8_t unget;
  unsigned long alloc;
  uint8_t buffincrement;
} file_t;
typedef struct file FILE;

typedef struct dirent {
  int id;
  char name[256];
  stat_t stat;
} dirent_t;

#endif

