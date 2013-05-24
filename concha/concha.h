/**
 *  concha/shell.h
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
#ifndef _CONCHA_H
#define _CONCHA_H

#include <stdint.h>

typedef struct {
  const char *name;
  int (*handler)(uint8_t argc, uint8_t **argv);
} sh_cmd_t;

void parse_cmd(char *str);

int command_cd(uint8_t argc, uint8_t **argv);
int command_cls(uint8_t argc, uint8_t **argv);
int command_help(uint8_t argc, uint8_t **argv);
int command_exit(uint8_t argc, uint8_t **argv);

#endif
