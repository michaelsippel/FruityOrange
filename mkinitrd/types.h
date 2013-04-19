/**
 *  mkinitrd/types.h
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
#ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

#define S_MODE_DIR 0x1
#define S_MODE_CHR 0x2
#define S_MODE_LNK 0x4

typedef struct initrd_inode {
  const char name[256];
  uint32_t mode;
  uint32_t length;
  
  uint32_t off;
  uint32_t parent_off;
} initrd_inode_t;

typedef struct initrd_dentry {
  const char name[256];
  uint32_t off;
} initrd_dentry_t;

#endif

