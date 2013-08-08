/**
 *  lib/user/stdio/file.c
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
#include <sys/file.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

FILE *fopen(const char path, char *modus) {
  FILE *file = malloc(sizeof(FILE));
  
  int m = 0;
  if(strchr(modus, "r") != NULL) { m = O_RDONLY; }
  if(strchr(modus, "w") != NULL) {
    if(m & O_RDONLY) {
      m = O_RDWR;
    } else {
      m = O_RDONLY;
    }
  }
  
  if(strchr(modus, "a") != NULL) { m |= O_APPEND; } 
  if(strchr(modus, "+") != NULL) {
    m |= O_RDWR;
  }
  
  file->handle = open(path, m, 0);
  file->fpos = 0;
  
  stat_t stat;
  fstat(file->handle, &stat);
  file->alloc = stat.size;  
  
  return file;
}

int fclose(FILE *file) {
  close(file->handle);
  free(file);
  return 0;
}

size_t fread(void *buf, size_t size, size_t n, FILE *file) {
  file->fpos += read(file->handle, buf, size * n);
  return n;
}

size_t fwrite(void *buf, size_t size, size_t n, FILE *file) {
  file->fpos += write(file->handle, buf, size * n);
  return n;
}

void fseek(FILE *file, int off, int whence) {
  switch(whence) {
    case SEEK_SET: // absolute
      file->fpos = off;
      break;
    case SEEK_CUR: // relative from current position
      file->fpos += off;
      break;
    case SEEK_END: // relative from end
      file->fpos = file->alloc - off;
      break;
  }
  lseek(file->handle, off, whence);
}

int ftell(FILE *file) {
  return (int)file->fpos;
}

