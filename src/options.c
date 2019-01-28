/* Print log file lines that have not been read.
 * 
 * Copyright (C) 2008-2019 Anders Lövgren, BMC-IT and Nowise Systems
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * ---
 * Send questions to: Anders Lövgren <andlov@nowise.se>
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#define _GNU_SOURCE
#include <stdio.h>
#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
# include <string.h>
#endif
#include <getopt.h>
#ifdef HAVE_LIMITS_H
# include <limits.h>
#endif

#include "logtail2.h"

static void usage(const char *prog, struct options *opts)
{
	printf("%s - Print log file lines that have not been read.\n", prog);
	printf("Usage: %s [-t] -f logfile [-o offsetfile]\n", prog);
	printf("Options:\n");
	printf("  -f logfile:     logfile to read after offset.\n");
	printf("  -o offsetfile:  offsetfile stores offset of previous run.\n");
	printf("  -t:             test mode, do not change offset in offsetfile.\n");
	if(!opts->compat) {
		printf("  -c, --compat:   enable logtail compatible mode.\n");
		printf("  -d, --debug:    enable debug output.\n");
		printf("  -h, --help:     show this help.\n");
		printf("  -V, --version:  show version info.\n");
	}
	printf("Send bugs to %s\n", PACKAGE_BUGREPORT);
}

static void version(const char *prog)
{
	printf("%s version %s\n", prog, PACKAGE_VERSION);
}

/*
 * Parse command line options.
 */
void parse_options(int argc, char **argv, struct options *opts)
{
	static struct option lopts[] = {
		{ "file",    1, 0, 'f' },
		{ "offset",  1, 0, 'o' },
		{ "test",    0, 0, 't' },
		{ "help",    0, 0, 'h' },
		{ "version", 0, 0, 'V' },
		{ "debug",   0, 0, 'd' },
		{ "compat",  0, 0, 'c' }
	};			
	int c, index;
	
	if(strcmp(opts->prog, "logtail") == 0) {
		opts->compat = 1;
	}
	if(argc < 2) {
		usage(opts->prog, opts);
		exit(PROG_ARGUMENT_ERROR);
	}
	
	while((c = getopt_long(argc, argv, "cdf:ho:tV", lopts, &index)) != -1) {
		switch(c) {
		case 'c':
			opts->compat = 1;
			break;
		case 'd':
			opts->debug++;
			break;
		case 'f':
			if(opts->logfile) {
				free(opts->logfile);
			}
			opts->logfile = malloc(strlen(optarg) + 1);
			if(!opts->logfile) {
				fprintf(stderr, "%s: failed alloc memory\n", opts->prog);
				exit(FAILED_ALLOC_MEMORY);
			}
			strcpy(opts->logfile, optarg);
			break;
		case 'h':
			usage(opts->prog, opts);
			exit(0);
			break;
		case 'o':
			if(opts->offset) {
				free(opts->offset);
			}
			opts->offset = malloc(strlen(optarg) + 1);
			if(!opts->offset) {
				fprintf(stderr, "%s: failed alloc memory\n", opts->prog);
				exit(FAILED_ALLOC_MEMORY);
			}
			strcpy(opts->offset, optarg);			
			break;
		case 't':
			opts->test = 1;
			break;
		case 'V':
			version(opts->prog);
			exit(0);
			break;
		}
	}
	
	if(!opts->logfile) {
		fprintf(stderr, "%s: missing required option '-f'\n", opts->prog);
		exit(PROG_ARGUMENT_ERROR);
	}
	if(!opts->offset) {
		set_offset_path(opts);
	}

	if(opts->debug) {
		printf("debug: using %s\n", opts->offset);
	}
}
