/************************************************************************ 
 * 
 * fstest - Vector data structure
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <assert.h>

#include "vector.h"

struct Vector {
    size_t allocated;
    size_t used;
    void   **data;
};

Vector *vector_create(void) {
    Vector *v = malloc(sizeof(Vector));
    if (v == NULL) {
	perror("vector_create() malloc");
	exit(1);
    }
    v->allocated = 1;
    v->used = 0;
    v->data = malloc(sizeof(void*));;
    if (v->data == NULL) {
	perror("vector_create() malloc2");
	exit(1);
    }
    return v;
}

void vector_destroy(Vector *v) {
    free(v->data);
    v->allocated = 0;
    v->used = 0;
    v->data = NULL;
    free(v);
}

void vector_add(Vector *v, void* data) {
    if (v->allocated == v->used) {
	void* t = realloc(v->data, sizeof(void*) * v->allocated * 2);
	if (t == NULL) {
	    perror("vector_add() realloc");
	    exit(1);
	}
	v->data = t;
	v->allocated *= 2;
    }
    v->data[v->used++] = data;
}

void vector_remove(Vector *v, size_t pos) {
    assert(pos < v->used);
    v->data[pos] = v->data[--v->used];
}

size_t vector_size(Vector *v) {
    return v->used;
}

void vector_clear(Vector *v) {
    v->used = 0;
}

void* vector_at(Vector *v, size_t pos) {
    assert(pos < v->used);
    return v->data[pos];
}
