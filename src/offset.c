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
#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
# include <string.h>
#endif
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <pwd.h>
#include <errno.h>
#ifdef HAVE_LIMITS_H
# include <limits.h>
#endif

#include "logtail2.h"

/*
 * Set logtail compatible offset file path.
 */
static void set_compat_offset_path(struct options *opts)
{
	struct stat st;
	
	opts->offset = malloc(PATH_MAX + 1);
	if(!opts->offset) {
		fprintf(stderr, "%s: failed alloc memory\n", opts->prog);
		exit(FAILED_ALLOC_MEMORY);
	}
	snprintf(opts->offset, PATH_MAX, "%s.offset", opts->logfile);
	
	if(stat(opts->offset, &st) < 0) {
		if(errno != ENOENT) {
			fprintf(stderr, "%s: failed stat offset file %s (%s)\n", 
				opts->prog, opts->offset, strerror(errno));
			exit(FAILED_WRITE_OFFSET);
		}
	} else {
		if(!S_ISREG(st.st_mode)) {
			fprintf(stderr, "%s: offset file %s is not a regular file.\n",
				opts->prog, opts->offset);
			exit(FAILED_WRITE_OFFSET);
		}
	}
}

/*
 * Set offset file path for user root.
 */
static void set_root_offset_path(struct options *opts)
{
	struct stat st;
	char *pp;
		
	opts->offset = malloc(PATH_MAX + 1);
	if(!opts->offset) {
		fprintf(stderr, "%s: failed alloc memory\n", opts->prog);
		exit(FAILED_ALLOC_MEMORY);
	}
	if(stat(OFFSET_CACHE_DIR, &st) < 0) {
		if(errno == ENOENT) {
			if(mkdir(OFFSET_CACHE_DIR, 0750) < 0) {
				fprintf(stderr, "%s: failed create cache directory %s (%s)\n",
					opts->prog, OFFSET_CACHE_DIR, strerror(errno));
				exit(FAILED_WRITE_OFFSET);
			}
		} else {
			fprintf(stderr, "%s: failed stat cache directory %s (%s)\n",
				opts->prog, OFFSET_CACHE_DIR, strerror(errno));
			exit(FAILED_WRITE_OFFSET);
		}
	}
	snprintf(opts->offset, PATH_MAX, "%s/%s.offset", OFFSET_CACHE_DIR, opts->logfile);
	pp = opts->offset + strlen(OFFSET_CACHE_DIR);
	if(*pp != '/') {
		*pp++ = '/';
	}
	while(*pp) {			    
		if(*pp == '/') {
			*pp = '_';
		}
		++pp;
	}
}

/*
 * Set offset file path for ordinary users.
 */
static void set_user_offset_path(struct options *opts)
{
	struct stat st;
	struct passwd *pwent;
	char *pp, *ps, *pr;
	size_t bytes, size;
	
	pwent = getpwuid(getuid());
	if(!pwent) {
		fprintf(stderr, "%s: failed lookup home directory path (%s)\n",
			opts->prog, strerror(errno));
		exit(USERDB_LOOKUP_ERROR);
	}
	
	size = 2 * PATH_MAX + 1;
	opts->offset = malloc(size);
	if(!opts->offset) {
		fprintf(stderr, "%s: failed alloc memory\n", opts->prog);
		exit(FAILED_ALLOC_MEMORY);
	}			
	
	ps = opts->offset;
	bytes = snprintf(ps, PATH_MAX, "%s/.%s", pwent->pw_dir, PACKAGE_NAME);
	if(bytes < 0) {
		fprintf(stderr, "%s: failed create path of cache directory (%s)\n",
			opts->prog, strerror(errno));
		exit(FAILED_WRITE_OFFSET);
	}
	size -= bytes;
	ps += bytes;
	if(stat(opts->offset, &st) < 0) {
		if(errno == ENOENT) {
			if(mkdir(opts->offset, 0750) < 0) {
				fprintf(stderr, "%s: failed create cache directory %s (%s)\n",
					opts->prog, opts->offset, strerror(errno));
				exit(FAILED_WRITE_OFFSET);
			}
		} else {
			fprintf(stderr, "%s: failed stat cache directory %s (%s)\n",
				opts->prog, opts->offset, strerror(errno));
			exit(FAILED_WRITE_OFFSET);
		}
	}
	pp = ps + 1;
#ifdef HAVE_REALPATH
	pr = realpath(opts->logfile, NULL);
#else
	if(strstr(opts->logfile, "..")) {
		fprintf(stderr, "%s: '..' is not allowed in log file path.\n", 
			opts->prog);
		exit(PROG_ARGUMENT_ERROR);
	}
	if(opts->logfile[0] != '/') {
		fprintf(stderr, "%s: relative pathes is not allowed in log file path.\n", 
			opts->prog);
		fprintf(stderr, "%s: use compatibility mode '-c' to allow relative pathes.\n",
			opts->prog);
		exit(PROG_ARGUMENT_ERROR);
	}
#endif  /* !HAVE_REALPATH */
	
	bytes = snprintf(ps, size, "%s.offset", pr);
	if(bytes < 0) {
		fprintf(stderr, "%s: failed create path of cache directory (%s)\n",
			opts->prog, strerror(errno));
		exit(FAILED_WRITE_OFFSET);
	}
	
	while(*pp) {
		if(*pp == '/') {
			*pp = '_';
		}
		++pp;
	}
	free(pr);
}

/*
 * Use system cache if running as root, otherwise use personal 
 * folder (in the home directory). If compatibility mode is used,
 * then we makes the path next to the logfile to monitor.
 */
void set_offset_path(struct options *opts)
{
	if(opts->compat) {
		set_compat_offset_path(opts);
	} else if(getuid() == 0 || geteuid() == 0) {
		set_root_offset_path(opts);
	} else {
		set_user_offset_path(opts);
	}
}
