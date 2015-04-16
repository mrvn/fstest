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

#ifndef VECTOR_H
#define VECTOR_H

typedef struct Vector Vector;

Vector *vector_create(void);
void vector_destroy(Vector *v);
void vector_add(Vector *v, void *data);
void vector_remove(Vector *v, size_t pos);
size_t vector_size(Vector *v);
void vector_clear(Vector *v);
void* vector_at(Vector *v, size_t pos);

#endif // #ifndef LIST_H
