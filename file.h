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

#ifndef FILE_H
#define FILE_H

#include "dir.h"

typedef struct File File;

void file_init(void);
File *file_create(Dir *dir, off_t size);
void file_destroy(File *f);
void file_destroy_all(void);
void file_check(File *f);
size_t file_num_files(void);
File* file_at(size_t pos);
off_t file_size(File *f);
void file_remove(size_t pos);

#endif // #ifndef FILE_H
