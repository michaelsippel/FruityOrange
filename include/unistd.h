/**
 *  include/unistd.h
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
#ifndef _UNISTD_H
#define _UNISTD_H

#include <stdint.h>

#define R_OK 0x1
#define W_OK 0x2
#define X_OK 0x4
#define F_OK 0x8

#define SEEK_SET 0x1
#define SEEK_CUR 0x2
#define SEEK_END 0x3

#define O_RDONLY 0x01 /* read only */
#define O_WRONLY 0x02 /* write only */
#define O_RDWR   0x04 /* read and write */
#define O_APPEND 0x08 /* append at end of file */
#define O_TRUNC  0x10 /* clear the file */
#define O_CREAT  0x20 /* create the file */
#define O_EXCL   0x40 /* forced file creation. if it already exist: fail */

#ifndef _NO_USER_INC
#include "user/unistd.h"
#endif

#endif

