/* Print log file lines that have not been read.
 * Copyright (C) 2008  Anders Lövgren
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
 * Send questions to: Anders Lövgren <anders.lovgren@bmc.uu.se>
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif
#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <errno.h>

#include "logtail2.h"

/*
 * Read saved offset from path.
 */
static void read_offset(const char *path, long *offset, struct options *opts)
{
	struct stat st;
	FILE *fs;
	
	if(stat(path, &st) < 0) {
		if(errno == ENOENT) {
			/*
			 * OK for offset file to be missing.
			 */
			return;
		} else {
			fprintf(stderr, "%s: failed stat offset file %s (%s)\n", 
				opts->prog, path, strerror(errno));
			exit(FAILED_STAT_LOGFILE);
		}
	}
	
	fs = fopen(path, "r");
	if(!fs) {
		fprintf(stderr, "%s: failed open offset file %s for reading (%s)\n", 
			opts->prog, path, strerror(errno));
		fclose(fs);
		exit(FAILED_STAT_LOGFILE);
	}
	if(fscanf(fs, "%lu\n", offset) != 1) {
		fprintf(stderr, "%s: failed read saved position from %s (%s)\n", 
			opts->prog, path, strerror(errno));
		fclose(fs);
		exit(FAILED_STAT_LOGFILE);
	}
	fclose(fs);
}

/*
 * Write current offset to path.
 */
static void write_offset(const char *path, long offset, struct options *opts)
{
	FILE *fs;
	
	fs = fopen(path, "w");
	if(!fs) {
		fprintf(stderr, "%s: failed open offset file %s for writing (%s)\n", 
			opts->prog, path, strerror(errno));
		exit(FAILED_WRITE_OFFSET);
	}
	fprintf(fs, "%lu\n", offset);
	fclose(fs);
}

/*
 * Read logfile based on options in opts and write to stdout. Optionally
 * save offset for future read if opts->test is false.
 */
void read_logfile(struct options *opts)
{
	FILE *fs;
	struct stat st;
	int c;
	
	read_offset(opts->offset, &opts->spos, opts);
	if(opts->debug && opts->spos) {
		printf("debug: read offset %lu\n", opts->spos);
	}

	if(stat(opts->logfile, &st) < 0) {
		fprintf(stderr, "%s: failed stat logfile file %s (%s)\n", 
			opts->prog, opts->logfile, strerror(errno));
		exit(FAILED_STAT_LOGFILE);
	}
	
	if(opts->spos > st.st_size) {
		if(opts->debug) {
			printf("debug: logfile has wrapped (spos %d bytes > size %d bytes)\n",
			       opts->spos, st.st_size);
		}
		opts->spos = 0;     /* wrapped logfile */
	}
	
	fs = fopen(opts->logfile, "r");
	if(!fs) {
		fprintf(stderr, "%s: failed open log file %s for reading (%s)\n",
			opts->prog, opts->logfile, strerror(errno));
		exit(FAILED_READ_LOGFILE);
	}
	if(opts->spos) {
		if(fseek(fs, opts->spos, SEEK_SET) < 0) {
			fprintf(stderr, "%s: failed seek last read position in log file (%s)\n",
				opts->prog, strerror(errno));
			fclose(fs);
			exit(FAILED_READ_LOGFILE);
		}
		if(opts->debug) {
			printf("debug: skipped %d bytes, current pos: %d\n", opts->spos, ftell(fs));
		}
	}

	while((c = getc(fs)) != EOF) {
		putchar(c);
	}	
	opts->cpos = ftell(fs);
	fclose(fs);	

	if(!opts->test) {
		if(opts->debug) {
			printf("debug: read %d bytes\n", opts->cpos - opts->spos);
		}
		if(opts->cpos > opts->spos) {
			if(opts->debug) {
				printf("debug: saving offset %d\n", opts->cpos);
			}
			write_offset(opts->offset, opts->cpos, opts);
		}
	}
}
