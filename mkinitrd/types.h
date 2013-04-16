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

#define S_MODE_DIR 0x1
#define S_MODE_CHR 0x2
#define S_MODE_LNK 0x4

#define S_MAO 0x4 // mode: access offset

#define S_IRUSR (0x001 << S_MAO) //user-read
#define S_IWUSR (0x002 << S_MAO) // -write
#define S_IXUSR (0x004 << S_MAO) // -execute
#define S_IRGRP (0x008 << S_MAO) //group-read
#define S_IWGRP (0x010 << S_MAO) // -write
#define S_IXGRP (0x020 << S_MAO) // -execute
#define S_IROTH (0x040 << S_MAO) //other-read
#define S_IWOTH (0x080 << S_MAO) // -write
#define S_IXOTH (0x100 << S_MAO) // -execute

#define S_ISREG(x) ((x.mode & S_MODE_DIR) ? 0 : 1)
#define S_ISDIR(x) ((x.mode & S_MODE_DIR) ? 1 : 0)
#define S_ISCHR(x) ((x.mode & S_MODE_CHR) ? 1 : 0)
#define S_ISBLK(x) ((x.mode & S_MODE_CHR) ? 0 : 1)
#define S_ISLNK(x) ((x.mode & S_MODE_LNK) ? 1 : 0)

typedef struct initrd_inode {
  const char name[256];
  int mode;
  int id;
  int length;
} initrd_inode_t;

typedef struct initrd_dentry {
  const char name[256];
  id_t id;
} initrd_dentry_t;

#endif

