/* stdout_dump
	- dump interface statistics in to stdout

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

#ifndef STDOUT_DUMP_H
#define STDOUT_DUMP_H

#include "speed.h"
#include "interfaces.h"

int init_stdout_dump(const int zero, const speed_unit_t s_unit);
int dump_stat_stdout(net_iface_t* interface);
void cleanup_stdout();

#endif

