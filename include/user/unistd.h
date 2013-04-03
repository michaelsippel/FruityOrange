/**
 *  include/user/unistd.h
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
#ifndef _USER_UNISTD_H
#define _USER_UNISTD_H

#include <time.h>
#include <stddef.h>
#include <sys/stat.h>

#define SEEK_SET 0x1
#define SEEK_CUR 0x2
#define SEEK_END 0x3

int usleep(long usec);
int sleep(long sec);

int open(const char *path, int oflags, mode_t mode);
void close(int fd);
int write(int fd, const void *buf, size_t len);
int read(int fd, const void *buf, size_t len);
int lseek(int fd, int off, int whence);

#endif
