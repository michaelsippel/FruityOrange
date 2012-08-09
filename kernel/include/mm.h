/**
 *  kernel/include/mm.h
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
#ifndef _MM_H
#define _MM_H

#include <stdint.h>
#include <multiboot.h>

#define PMM_FREE 1
#define PMM_USED 0
#define PAGE_SIZE 0x1000

void init_pmm(struct multiboot_info *mb_info);
void* pmm_alloc(void);
void pmm_free(void *ptr);
void pmm_mark_used(void *ptr);

#endif
