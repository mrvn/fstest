/************************************************************************ 
 * 
 * fstest - File data structure
 * 
 * Authors: Goswin von Brederlow <brederlo@informatik.uni-tuebingen.de>
 * 
 * Copyright (C) 2007, 2008 Q-leap Networks, Goswin von Brederlow 
 * 
 *    This program is free software; you can redistribute it and/or modify 
 *    it under the terms of the GNU General Public License as published by 
 *    the Free Software Foundation; either version 2 of the License, or 
 *    (at your option) any later version. 
 * 
 *    This program is distributed in the hope that it will be useful, 
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details. 
 * 
 *    You should have received a copy of the GNU General Public License 
 *    along with this program; if not, write to the Free Software 
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 
 *    USA 
 * 
 ************************************************************************/

#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "vector.h"
#include "dir.h"
#include "file.h"

const off_t BUF_SIZE = 1024 * 1024;

struct File {
    Dir *dir;
    off_t size;
    uint32_t id;
};

Vector *all_files;

void file_init(void) {
    all_files = vector_create();
}

const char* file_path(File *f) {
    static char buf[PATH_MAX];
    snprintf(buf, PATH_MAX, "%s/%08x", dir_path(f->dir), f->id);
    return buf;
}

File *file_create(Dir *dir, off_t size) {
    int fd;
    File *f = malloc(sizeof(File));
    if (f == NULL) {
	perror("file_create() malloc");
	exit(1);
    }
    f->dir = dir;
    f->size = size;
retry:
    f->id = random();
    // fprintf(stderr, "Creating file %s %ld\n", file_path(f), size);

    if ((fd = open(file_path(f), O_WRONLY | O_CREAT | O_EXCL, 0600)) == -1) {
	if (errno == EEXIST) goto retry; // Try again with new name
	perror(file_path(f));
	exit(1);
    }

    char buf[BUF_SIZE];
    uint32_t *t = (uint32_t*)buf;
    off_t i;
    uint32_t val = f->id;
    for(i = 0; i < (off_t)(BUF_SIZE / sizeof(uint32_t)); ++i) {
	*t++ = val++;
    }

    // write file
    for(i = 0; i < size; i += BUF_SIZE) {
	off_t offset = 0;
	while(offset < BUF_SIZE) {
	    ssize_t len;
	    if ((len = write(fd, &buf[offset], BUF_SIZE - offset)) < 0) {
		perror(file_path(f));
		exit(1);
	    }
	    offset += len;
	}
    }

    close(fd);

    vector_add(all_files, f);
    dir_add_file(dir);

    return f;
}

void file_destroy(File *f) {
    //fprintf(stderr, "Destroying file %s\n", file_path(f));
    dir_remove_file(f->dir);
    if (unlink(file_path(f)) != 0) {
	perror(file_path(f));
	exit(1);
    }
}

void file_destroy_all(void) {
    size_t i;
    for(i = 0; i < vector_size(all_files); ++i) {
	file_destroy((File*)vector_at(all_files, i));
    }
    vector_clear(all_files);
}

void file_check(File *f) {
    int fd;

    // Open file
    if ((fd = open(file_path(f), O_RDONLY)) == -1) {
	perror(file_path(f));
	exit(1);
    }

    // Create buffer and fill with id
    char bufm[BUF_SIZE];
    char buff[BUF_SIZE];
    uint32_t *t = (uint32_t*)bufm;
    off_t i;
    uint32_t val = f->id;
    for(i = 0; i < (off_t)(BUF_SIZE / sizeof(uint32_t)); ++i) {
	*t++ = val++;
    }

    // read and compare file
    for(i = 0; i < f->size; i += BUF_SIZE) {
	off_t offset = 0;
	while(offset < BUF_SIZE) {
	    ssize_t len;
	    if ((len = read(fd, &buff[offset], BUF_SIZE - offset)) < 0) {
		perror(file_path(f));
		exit(1);
	    }
	    offset += len;
	}
	if (memcmp(bufm, buff, BUF_SIZE) != 0) {
	    fprintf(stderr, "File corruption in %s around %ld [pattern = %08x]\n", file_path(f), i, f->id);
	    exit(1);
	}
    }

    close(fd);
}

size_t file_num_files(void) {
    return vector_size(all_files);
}

File* file_at(size_t pos) {
    return (File*)vector_at(all_files, pos);
}

off_t file_size(File *f) {
    return f->size;
}

void file_remove(size_t pos) {
    vector_remove(all_files, pos);
}
