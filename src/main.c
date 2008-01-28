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

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
# include <string.h>
#endif
#include <libgen.h>

#ifndef HAVE_MEMSET
# ifdef HAVE_MEMORY_H
#  include <memory.h>      /* memset */
# endif
#endif

#include "logtail2.h"

struct options opts;

#ifdef HAVE_ATEXIT
static void exit_handler(void)
{
	if(opts.logfile) {
		free(opts.logfile);
		opts.logfile = NULL;
	}
	if(opts.offset) {
		free(opts.offset);
		opts.offset = NULL;
	}
}
#endif

#include <stdio.h>

int main(int argc, char **argv)
{	
	memset(&opts, 0, sizeof(struct options));
	opts.prog = basename(argv[0]);

#ifdef HAVE_ATEXIT
	atexit(exit_handler);
#endif
	
	parse_options(argc, argv, &opts);
	read_logfile(&opts);
	
	return 0;
}
