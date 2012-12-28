/* speed
	- useful datatypes and methods for speed calculations and representations

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

#ifndef SPEED_H
#define SPEED_H

#include <stdint.h>

#define START_CALENDAR_DATE 	1900
#define MONTH_OFFSET 		1

enum speed_unit {
	BIT_PER_SEC = 0,
	BYTE_PER_SEC,
	KILOBIT_PER_SEC,
	KILOBYTE_PER_SEC,
	MEGABIT_PER_SEC,
	MEGABYTE_PER_SEC,
	GIGABIT_PER_SEC,
	GIGABYTE_PER_SEC
};
typedef enum speed_unit speed_unit_t;

static const char* speed_values[] = {
	"Bits/sec",
	"Bytes/sec",
	"kBits/sec",
	"kBytes/sec",
	"MBits/sec",
	"MBytes/sec",
	"GBits/sec",
	"GBytes/sec"
};

static const char* data_amounts[] = {
	"Bits",
	"Bytes",
	"kBits",
	"kBytes",
	"MBits",
	"MBytes",
	"GBits",
	"GBytes"
};

static float value_in_actual_unit(float valBytes, speed_unit_t unit)
{
	if (unit == BYTE_PER_SEC) {
		return valBytes;
	}

	switch (unit) {
		case BIT_PER_SEC:
			return valBytes * 8;

		case KILOBIT_PER_SEC:
			return (valBytes / 1024) * 8;

		case KILOBYTE_PER_SEC:
			return valBytes / 1024;

		case MEGABIT_PER_SEC:
			return (valBytes / (1024 * 1024)) * 8;

		case MEGABYTE_PER_SEC:
			return valBytes / (1024 * 1024);

		case GIGABIT_PER_SEC:
			return (valBytes / (1024 * 1024 * 1024)) * 8;

		case GIGABYTE_PER_SEC:
			return valBytes / (1024 * 1024 * 1024);

		default:
			break;
	}

	return valBytes;
}

static double calc_speed(const long long int start_bytes, const long long int end_bytes, const unsigned time_elapsed)
{
	uint64_t sub_bytes = 0;

	if (start_bytes == 0) {
		return 0;
	}

	sub_bytes = end_bytes - start_bytes;

	return sub_bytes / time_elapsed;
}

#endif

