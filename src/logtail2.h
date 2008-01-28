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

#ifndef __LOGTAIL2_H__
#define __LOGTAIL2_H__

/*
 * Non-zero exit codes, values >= 80 are extensions:
 */
#define FAILED_STAT_LOGFILE  65      /* also fail offset file */
#define FAILED_READ_LOGFILE  66
#define FAILED_WRITE_OFFSET  73
#define FAILED_ALLOC_MEMORY  80
#define PROG_ARGUMENT_ERROR  81      /* program argument error */
#define USERDB_LOOKUP_ERROR  82      /* failed lookup user */

#define OFFSET_CACHE_DIR     "/var/cache/" PACKAGE_NAME

#ifndef HAVE_LIMITS_H
# define PATH_MAX 512
#endif
#ifndef HAVE_SYS_TYPES_H
# ifndef size_t
typedef unsigned int size_t;
# endif
#endif
#ifndef HAVE_STRERROR
# define strerror(code) (sys_errlist[code])
#endif

struct options
{
	char *prog;            /* program name */
	char *logfile;         /* logfile */
	char *offset;          /* offset file */
	long spos;             /* saved position */
	long cpos;             /* current position */
	int test;              /* enable dry-run */
	int debug;             /* enable debug */
	int compat;            /* enable logtail compatibility */
};

void parse_options(int argc, char **argv, struct options *opts);
void read_logfile(struct options *opts);
void set_offset_path(struct options *opts);

#endif /* #define __LOGTAIL2_H__ */
