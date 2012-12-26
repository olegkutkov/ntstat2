/* main.c

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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include "interfaces.h"
#include "config.h"
#include "timer.h"


/*
 */
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
	"Bit/s",
	"Byte/s",
	"kBit/s",
	"kByte/s",
	"MBit/s",
	"MByte/s",
	"GBit/s",
	"GByte/s"
};

/*
 */
static const char* working_animation_chars[] = {
	"\\",
	"/",
	"-"
};

enum working_animation_phases {
	PHASE_ONE = 0,
	PHASE_TWO,
	PHASE_THREE,
	PHASE_LAST
};
typedef enum working_animation_phases working_animation_phases_t;

static working_animation_phases_t last_animation_phase = PHASE_ONE;

/**/

#define START_DATE 1900
#define MONTH_OFFSET 1

static int run_stat_for_iface(const char* ifname);
static speed_unit_t s_speed_unit = KILOBYTE_PER_SEC;  /* default speed unit */
static time_t s_measure_timeout = 1000;			/* default timout 1000ms */
static int use_raw_format = 0;
static int start_from_zero = 0;
static char raw_file_name[1024] = { 0 };

/* 
 */

static void show_usage()
{
	printf("%s usage:\n\n", PACKAGE_NAME);
	printf("\t-h\t\tShow this help and exit\n");
	printf("\t-v\t\tShow program version and exit\n");
	printf("\t-l\t\tList all available interfaces\n");
	printf("\t-i <name>\tSpecify interface name for listening\n");
	printf("\t-t <ms>\t\tSets update interval in milliseconds\n");
	printf("\t-u\t\tSets the type of unit used for the display (b|B|k|K|m|M|g|G)\n");
	printf("\t-w <file>\tWrite all data in raw format in specified file\n");
	printf("\n");
}

static void show_version()
{
	printf("%s %s\n", PACKAGE_NAME, PACKAGE_VERSION);
	printf("%s\n", PACKAGE_URL);
	printf("Please report all bugs to: %s\n", PACKAGE_BUGREPORT);
}

static speed_unit_t char_to_speed_unit(const char c)
{
	switch (c) {
		case 'b':
			return BIT_PER_SEC;

		case 'B':
			return BYTE_PER_SEC;

		case 'k':
			return KILOBIT_PER_SEC;

		case 'K':
			return KILOBYTE_PER_SEC;

		case 'm':
			return MEGABIT_PER_SEC;

		case 'M':
			return MEGABYTE_PER_SEC;

		case 'g':
			return GIGABIT_PER_SEC;

		case 'G':
			return GIGABYTE_PER_SEC;

		default:
			break;
	}

	return KILOBYTE_PER_SEC;
}

int main(int argc, char **argv)
{
	int c;
	const char* ifname = NULL;

	while ((c = getopt (argc, argv, "li:w:t:u:hvz")) != -1) {
		switch (c) {
			case 'l':
				dump_interfaces();
				return 0;

			case 'i':
				ifname = optarg;
				break;

			case 'h':
				show_usage();
				return 0;

			case 'v':
				show_version();
				return 0;

			case 'u':
				s_speed_unit = char_to_speed_unit(optarg[0]);
				break;

			case 't':
				s_measure_timeout = (time_t)atoi(optarg);

			case 'w':
				use_raw_format = 1;
				strcpy(raw_file_name, optarg);
				break;

			case 'z':
				start_from_zero = 1;
				break;

			case '?':
				return -1;
		}
	}

	run_stat_for_iface(ifname);

	return 0;
}

static void get_date_time_str(char* prefix, char* str)
{
	struct tm* current_tm;
	time_t time_now;

	time(&time_now);
	current_tm = localtime(&time_now);

	sprintf( str
		, "%s %04i-%02i-%02i %02i:%02i:%02i"
		, prefix
		, current_tm->tm_year + START_DATE
		, current_tm->tm_mon + MONTH_OFFSET
		, current_tm->tm_mday
		, current_tm->tm_hour
		, current_tm->tm_min
		, current_tm->tm_sec );
}

static void timer_function(void* cookie)
{
	if_stat_t* stat = NULL;
	net_iface_t* iface = (net_iface_t*) cookie;

	if (!iface) {
		return;
	}

	stat = &iface->stat;
	iface->get_stat(stat, iface->if_name);

	if (iface->dump_stat(iface) == -1) {
		printf("Unable to dump statistics. It's better to abort application!\n");
		abort();
	}

/*
	get_date_time_str("\rNow: ", date_time_str);

	printf("\r| %s %-12lli %-10lli %-13lli %-9lli"
		, date_time_str
		, stat->out_bytes
		, stat->out_bytes/1024
		, stat->in_bytes
		, stat->in_bytes/1024);
*/
}

static int dump_stat_stdout(net_iface_t* interface)
{
	return 0;
}

static int dump_stat_raw_file(net_iface_t* interface)
{
	printf("\rWorking...  %s", working_animation_chars[last_animation_phase++]);

	if (last_animation_phase == PHASE_LAST) {
		last_animation_phase = PHASE_ONE;
	}

	return 0;
}

static void wait_for_shutdown()
{
	static struct termios termio_prev, termio_curr;

	tcgetattr(STDIN_FILENO, &termio_prev);
	termio_curr = termio_prev;

	termio_curr.c_lflag &= ~(ICANON);
	termio_curr.c_lflag &= ~(ECHO);

	tcsetattr(STDIN_FILENO, TCSANOW, &termio_curr);

	while (getchar() != 'c') {
		usleep(250000);
	}

	tcsetattr(STDIN_FILENO, TCSANOW, &termio_prev);

	printf("\n\n  'c' key was pressed, exiting...\n");
}

static int run_stat_for_iface(const char* ifname)
{
	char date_time_str[52];
	net_iface_t interface;

	memset(&interface, 0, sizeof(net_iface_t));
	strcpy(interface.if_name, ifname);

	if (setup_interface(&interface) == -1) {
		printf("Failed to setup interface %s\n", ifname);
		return -1;
	}

	if (use_raw_format) {
		printf(" * Set RAW mode, output file: %s\n", raw_file_name);
		interface.dump_stat = dump_stat_raw_file;
	} else {
		interface.dump_stat = dump_stat_stdout;
	}

	set_timer_callback(timer_function);

	get_date_time_str("Started at: ", date_time_str);
	printf("\n * %s\n\n", date_time_str);

	printf(" * Listening on %s\n", ifname);
	printf("\tinet: %s\n\tinet6: %s\n\n", interface.str_ip4_addr, interface.str_ip6_addr);

	if (start_timer(s_measure_timeout, &interface) == -1) {
		printf("Unable to start\n");
	}

	wait_for_shutdown();

	stop_timer();

	return 0;
}

