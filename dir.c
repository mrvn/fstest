/************************************************************************ 
 * 
 * fstest - Directory data structure
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

#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "options.h"
#include "list.h"
#include "vector.h"
#include "file.h"
#include "dir.h"

struct Dir {
    List list;
    Dir *parent;
    char name[4];
    int num_files;
};

Dir root_dir;
Vector *active_dirs = NULL;

const char* dir_path(Dir *d) {
    static char buf[PATH_MAX];
    if (d == NULL) {
	strncpy(buf, root_path, PATH_MAX - 1);
        buf[PATH_MAX - 1] = 0;
    } else {
	dir_path(d->parent);
        size_t len = strlen(buf) + 1 + strlen(d->name) + 1;
        assert(len <= PATH_MAX);
	strcat(buf, "/");
	strcat(buf, d->name);
    }
    return buf;
}

Dir *dir_create(Dir *parent, int num) {
    // Allocate directory
    Dir *d = malloc(sizeof(Dir));
    if (d == NULL) {
	perror("dir_create() malloc");
	exit(1);
    }
    list_init(&d->list);
    d->parent = parent;
    d->name[0] = 'd';
    d->name[1] = '0' + num / 10;
    d->name[2] = '0' + num % 10;
    d->name[3] = 0;
    d->num_files = 0;

    // Create directory in filesystem
    //fprintf(stderr, "Creating dir %s\n", dir_path(d));
    if (mkdir(dir_path(d), 0700) != 0) {
	perror(dir_path(d));
	exit(1);
    }

    // Link into dir list and add to active list
    list_add_after(&root_dir.list, &d->list);
    vector_add(active_dirs, d);

    // Create subdirs
    while(--num > 0) {
	(void)dir_create(d, num);
    }

    // All done, return directory
    return d;
}

void dir_init() {
    list_init(&root_dir.list);
    root_dir.parent = NULL;
    root_dir.name[0] = 0;
    root_dir.num_files = 0;

    // Create directory in filesystem
    //fprintf(stderr, "Creating dir %s\n", root_path);
    if (mkdir(root_path, 0700) != 0) {
	perror(root_path);
	exit(1);
    }

    // Create active vector
    active_dirs = vector_create();
    vector_add(active_dirs, &root_dir);

    // Create subdir
    (void)dir_create(NULL, 1);
}

void dir_destroy(Dir *d) {
    //fprintf(stderr, "Destroying dir %s\n", dir_path(d));
    int res = rmdir(dir_path(d));
    if (res != 0 && errno != ENOENT) {
	perror(dir_path(d));
    }
}

void dir_destroy_all(void) {
    vector_clear(active_dirs);
    List *dir = root_dir.list.next;
    while(dir != NULL) {
	List* t = dir->next;
	// FIXME: use container
	dir_destroy((Dir*)dir);
	dir = t;
    }
    //fprintf(stderr, "Destroying dir %s\n", root_path);
    int res = rmdir(root_path);
    if (res != 0 && errno != ENOENT) {
	perror(root_path);
    }    
}

size_t dir_num_active(void) {
    return vector_size(active_dirs);
}

Dir *dir_active_at(size_t pos) {
    return (Dir*)vector_at(active_dirs, pos);
}

int dir_num_files(Dir *d) {
    return d->num_files;
}

void dir_remove_active(size_t pos) {
    vector_remove(active_dirs, pos);
}

void dir_activate_dirs(void) {
    List *dir = root_dir.list.next;
    while(dir != NULL) {
	List* t = dir->next;
	// FIXME: use container
	vector_add(active_dirs, (Dir*)dir);
	dir = t;
    }
}

void dir_add_file(Dir *d) {
    ++d->num_files;
}

void dir_remove_file(Dir *d) {
    --d->num_files;
}
