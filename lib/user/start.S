//
//  lib/user/start.S
//
//  (C) Copyright 2013 Michael Sippel
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
.section .text
.extern main
.extern init_heap

.global _start
_start:
    push %ebx // argv
    push %eax // argc
    call main
    movl %eax, %ebx
    movl $0, %eax
    int $0x30
    jmp _stop

_stop:
    jmp _stop

