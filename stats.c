/************************************************************************ 
 * 
 * fstest - Stats data structure
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

#include <time.h>
#include <stdio.h>
#include <inttypes.h>

#include "options.h"
#include "stats.h"

typedef struct {
    time_t time;
    uint64_t write, read, files;
} Stats;

Stats old, now;
time_t start_time;

void stats_init(void) {
    start_time = time(NULL);
    old = (Stats){0, 0, 0, 0};
    now = (Stats){time(NULL), 0, 0, 0};

    printf("Starting test at    : %s", ctime(&start_time));
    printf("<timestamp> write: <total> GiB, <total> MiB/s [<last> MiB/s] read: <total> GiB, <total> MiB/s [<last> MiB/s] # ctime\n");
}

uint64_t stats_files(void) {
    return now.files;
}

void stats_add_write(uint64_t n) {
    now.write += n;
}

void stats_add_read(uint64_t n) {
    now.read += n;
}

void stats_add_files(uint64_t n) {
    now.files += n;
}

void stats_output(void) {
    static const uint64_t MEGA = 1024 * 1024;
    static const uint64_t GIGA = MEGA * 1024;

    now.time = time(NULL);
    if (now.time - old.time > stats_interval) {
	double t = now.time - start_time;
	if (t == 0) t = 1;
	double twrite = now.write / t / MEGA;
	double tread = now.read / t / MEGA;
	t = now.time - old.time;
	double write = (now.write - old.write) / t / MEGA;
	double read = (now.read - old.read) / t / MEGA;
	printf("%ld write: %"PRIu64" GiB, %f MiB/s [%f MiB/s] read: %"PRIu64" GiB, %f MiB/s [%f MiB/s] # %s", now.time, now.write / GIGA, twrite, write, now.read / GIGA, tread, read, ctime(&now.time));
	old = now;
    }
}
