/************************************************************************ 
 * 
 * Filesystem stress and verify - main
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

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/statvfs.h>
#include <assert.h>
#include <inttypes.h>

#include "options.h"
#include "dir.h"
#include "file.h"
#include "stats.h"

uint64_t fssize = 0;

void cleanup(void) {
    file_destroy_all();
    dir_destroy_all();
}

void fstest(uint64_t goal) {
    int level = 1;
    int max_files = 1;
    bool was_full = false;

    while(true) {
	// Output stats
	stats_output();

	// Pick a random file size
	uint64_t size = 1ULL << (size_min + random() % (size_max - size_min + 1));

	// Check space
	while(true) {
	    struct statvfs vfsbuf;
	    if (statvfs(root_path, &vfsbuf) != 0) {
		perror("statvfs()");
		exit(1);
	    }
	    uint64_t fsfree = ((uint64_t)vfsbuf.f_bavail) * vfsbuf.f_frsize;

	    // Enough free already?
	    if (size < fsfree && fsfree - size / 2 > goal) {
		// remove 1 file in 2 untill the fs was full once
		if (was_full || stats_files() > file_num_files() * 2) break;
	    } else {
		if (!was_full) {
		    fprintf(stderr, "Goal of %f%% reached\n", 100 * percent);
		    was_full = true;
		}
	    }

	    // Remove some files
	    if (file_num_files() == 0) break; // no files left to delete
	    int num = random() % file_num_files();
	    File *f = file_at(num);
	    file_remove(num);
	    file_check(f);
	    stats_add_read(file_size(f));
	    file_destroy(f);
	}
    
	// Pick a random directory
	int num = random() % dir_num_active();
	Dir *d = dir_active_at(num);

	// Create file
	(void)file_create(d, size);
	stats_add_write(size);
	stats_add_files(1);

	// Remove dir from active_dirs if full
	if (dir_num_files(d) >= max_files) {
	    dir_remove_active(num);
	}

	if (dir_num_active() == 0) {
	    ++level;
	    max_files = level * level;
	    //fprintf(stderr, "Going to level %d\n", level);
	    dir_activate_dirs();
	    (void)dir_create(NULL, level);
	};
    }
}

int main(int argc, char * const argv[]) {
    struct statvfs statvfsbuf;

    options_init(argc, argv);

    printf("fstest v0.1\n");
    printf("Directory           : %s\n", root_path);
    printf("Goal percentage used: %f%%\n", percent * 100);
    printf("Minimum size        : 2^%d\n", size_min);
    printf("Maximum size        : 2^%d\n", size_max);
    printf("Stats time          : %ds\n", stats_interval);

    if (atexit(cleanup) != 0) {
	perror("atexit");
    }

    dir_init();
    file_init();

    // Get FS stats
    if (statvfs(root_path, &statvfsbuf) != 0) {
	perror("statvfs(): ");
	exit(1);
    }
    fssize = ((uint64_t)statvfsbuf.f_blocks) * statvfsbuf.f_frsize;
    uint64_t fsfree = ((uint64_t)statvfsbuf.f_bavail) * statvfsbuf.f_frsize;
    double used = (double)(fssize - fsfree) / fssize;
    printf("Filesystem size     : %"PRIu64"\n", fssize);
    printf("Filesystem free     : %"PRIu64"\n", fsfree);
    printf("Filesystem used     : %lf%%\n", 100 * used);
    if (used >= percent) {
	fprintf(stderr, "Error: Filesystem already above %% used goal: %lf >= %lf\n", 100 * used, 100 * percent);
	exit(1);
    }
    if (1LLU << size_max > fsfree / 2) {
	while (1LLU << size_max > fsfree / 2) {
	    --size_max;
	}
	printf("Warning: Maximum size excessive, trunking to 2^%d\n", size_max);
    }
    if (size_max < size_min) {
	fprintf(stderr, "Error: Maximum size can't be less than minimum size\n");
	exit(1);
    }
    stats_init();

    fstest((uint64_t)(fssize * (1 - percent)));

    return 0;
}
