/* raw_dump
	- dump interface statistics in to raw file

   Copyright 2012  Oleg Kutkov <elenbert@gmail.com>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "raw_dump.h"

/**/
static const char working_animation_chars[] = {
	'\\',
	'/',
	'-'
};

enum working_animation_phases {
	PHASE_ONE = 0,
	PHASE_TWO,
	PHASE_THREE,
	PHASE_LAST
};
typedef enum working_animation_phases working_animation_phases_t;
/**/

static working_animation_phases_t last_animation_phase = PHASE_ONE;
static int raw_file_fd = -1;

int init_raw_dump(const int zero, const speed_unit_t s_unit, const char* r_file)
{
	raw_file_fd = open(r_file, O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);

	if (raw_file_fd == -1) {
		printf("Failed to open file %s - error: %s\n", r_file, strerror(errno));
		return -1;
	}

	return 0;
}

int dump_stat_raw(net_iface_t* interface)
{
	printf("\r Working...  %c", working_animation_chars[last_animation_phase++]);

	if (last_animation_phase == PHASE_LAST) {
		last_animation_phase = PHASE_ONE;
	}

	return 0;
}

void cleanup_raw_dump()
{
	if (raw_file_fd != -1) {
		close(raw_file_fd);
		raw_file_fd = -1;
	}
}

