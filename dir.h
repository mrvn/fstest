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

#ifndef DIR_H
#define DIR_H

typedef struct Dir Dir;

void dir_init(void);
Dir *dir_create(Dir *parent, int num);
Dir *dir_create_root(void);
void dir_destroy(Dir *d);
void dir_destroy_all(void);
const char *dir_path(Dir *d);
size_t dir_num_active(void);
Dir *dir_active_at(size_t pos);
int dir_num_files(Dir *d);
void dir_remove_active(size_t pos);
void dir_activate_dirs(void);
void dir_add_file(Dir *d);
void dir_remove_file(Dir *d);

#endif // #ifndef DIR_H
