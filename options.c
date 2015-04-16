/************************************************************************ 
 * 
 * fstest - Global options and default values
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
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/limits.h>

#include "options.h"

double percent = 0.9;		// Goal is 90% full
int size_min = 20;		// 1 MiB
int size_max = 35;		// 32 GiB
int stats_interval = 900;	// 5 minutes
char *root_path = ".";		// current directory

void usage(FILE *f, const char* cmd) {
    fprintf(f, "%s -h|--help         - show help.\n", cmd);
    fprintf(f, "%s [options] [<dir>] - run filesystem test in <dir> [.]\n", cmd);
    fprintf(f, "\n");
    fprintf(f, "Options:\n");
    fprintf(f, " -p|--percent <percent> - goal percentage used of filesystem [90].\n");
    fprintf(f, " -i|--min-size <i>      - use minimum size 2^i [%d].\n", size_min);
    fprintf(f, " -a|--max-size <i>      - use maximum size 2^i [%d].\n", size_max);
    fprintf(f, " -t|--stats-time <i>    - report stats at most every i seconds [%d].\n", stats_interval);
}

void options_init(int argc, char * const argv[]) {
    const char *cmd = argv[0];
    // Getopt stuff
    const char optstring[] = "hp:i:a:t:";
    const struct option longopts[] = {
	{ "help", 0, NULL, 'h' },
	{ "percent", 1, NULL, 'p' },
	{ "min-size", 1, NULL, 'i' },
	{ "max-size", 1, NULL, 'a' },
	{ "stats-time", 1, NULL, 't' },
	{ NULL, 0, NULL, 0 }};
    int longindex = 0;
    int res;

    while((res = getopt_long(argc, argv, optstring, longopts, &longindex)) != -1) {
	switch(res) {
	case 'h':
	    usage(stdout, cmd);
	    exit(0);
	    break;
	case '?':
	    usage(stderr, cmd);
	    exit(1);
	    break;
	case 'p':
	    percent = strtod(optarg, NULL) / 100;
	    break;
	case 'i':
	    size_min = strtod(optarg, NULL);
	    break;
	case 'a':
	    size_max = strtod(optarg, NULL);
	    break;
	case 't':
	    stats_interval = strtod(optarg, NULL);
	    break;
	default:
	    printf ("Error: unknown option '%c'\n", res);
	    usage(stderr, cmd);
	    exit(1);
	}
    }

    // Get optional arg <dir>
    if (optind < argc) {
	root_path = argv[optind++];
    }

    if (optind < argc) {
	int i;
	fprintf(stderr, "Error: extra args:");
	for(i = optind; i < argc; ++i) {
	    fprintf(stderr, " '%s'", argv[i]);
	}
	fprintf(stderr, "\n");
	usage(stderr, cmd);
	exit(1);
    }

    // Check that dir exists and is a directory
    size_t len = strlen(root_path);
    if (len == 0) {
	fprintf(stderr, "directory specified as ''\n");
	exit(1);
    }
    if (root_path[len - 1] == '/') {
	// cut trailing /
	root_path[len - 1] = 0;
	--len;
    }
    struct stat statbuf;
    if (len > 0 && stat(root_path, &statbuf) != 0) {
	perror(root_path);
	exit(1);
    }
    if (len > 0 && !S_ISDIR(statbuf.st_mode)) {
	fprintf(stderr, "Error: %s is not a directory.\n", root_path);
	exit(1);
    }
    
    // add pid to directory
    pid_t pid = getpid();
    static char buf[PATH_MAX];
    snprintf(buf, PATH_MAX, "%s/fstest.%d", root_path, pid);
    root_path = buf;
}
