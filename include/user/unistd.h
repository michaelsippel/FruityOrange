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

#include <sys/types.h>
#include <stddef.h>
#include <time.h>

int usleep(long usec);
int sleep(long sec);

fd_t open(const char *path, int oflags, mode_t mode);
void close(fd_t fd);
int write(fd_t fd, const void *buf, size_t len);
int read(fd_t fd, void *buf, size_t len);
int lseek(fd_t fd, int off, int whence);
int chdir(const char *path);
char *getcwd(char *buf, size_t len);

#endif
