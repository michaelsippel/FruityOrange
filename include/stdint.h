/**
 *  kernel/stdint.h
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
#ifndef _STDINT_H
#define _STDINT_H

// Signed
typedef signed char		int8_t;
typedef short int		int16_t;
typedef int			int32_t;
//typedef long int		int64_t;
__extension__
typedef long long int		int64_t;

// Unsigned
typedef unsigned char		uint8_t;
typedef unsigned short int	uint16_t;
typedef unsigned int		uint32_t;
//typedef unsigned long int	uint64_t;
__extension__
typedef unsigned long long int	uint64_t;

typedef unsigned int		uintptr_t;


#endif
