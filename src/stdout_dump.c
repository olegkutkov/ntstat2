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

#include <stdio.h>
#include <time.h>
#include "stdout_dump.h"

static speed_unit_t s_unit_set;

static unsigned long start_data_rx = 0;
static unsigned long start_data_tx = 0;
static unsigned long total_data_rx = 0;
static unsigned long total_data_tx = 0;

static unsigned int total_packets_tx = 0;
static unsigned int total_packets_rx = 0;

static unsigned int total_dropped_packets_rx = 0;
static unsigned int total_dropped_packets_tx = 0;
static unsigned int total_errors_tx = 0;

static time_t start_time = 0;
static time_t end_time = 0;

void print_header(const speed_unit_t s_unit)
{
	printf("+---------------------+-----------------+--------------------+---------------------+----------------------+\n");
	printf("| Date/time           | Received %-6s | Transmitted %-6s | In speed %-9s | Out speed %-9s |\n",
			data_amounts[s_unit], data_amounts[s_unit], speed_values[s_unit], speed_values[s_unit]);
	printf("+---------------------+-----------------+--------------------+---------------------+----------------------+\n");
	fflush(stdout);
}

int init_stdout_dump(const int zero, const speed_unit_t s_unit)
{
	s_unit_set = s_unit;
	print_header(s_unit);
	return 0;
}

int dump_stat_stdout(net_iface_t* interface)
{
	time_t time_now = time(NULL);
	struct tm* current_tm = localtime(&time_now);

	if (start_data_rx == 0) {
		start_data_rx = interface->stat.in_bytes;
	}

	if (start_data_tx == 0) {
		start_data_tx = interface->stat.out_bytes;
	}

	if (start_time == 0) {
		start_time = time_now;
	}

	total_data_rx += interface->stat.in_bytes;
	total_data_tx += interface->stat.out_bytes;

	total_packets_tx += interface->stat.out_packets;
	total_packets_rx += interface->stat.in_packets;
	total_dropped_packets_rx += interface->stat.in_dropped;
	total_dropped_packets_tx += interface->stat.out_dropped;
	total_errors_tx += interface->stat.if_errors;

	printf("| %04i-%02i-%02i/%02i:%02i:%02i | %-15lli | %-18lli | %-19s | %-19s |\r"
		, current_tm->tm_year + START_CALENDAR_DATE
		, current_tm->tm_mon + MONTH_OFFSET
		, current_tm->tm_mday
		, current_tm->tm_hour
		, current_tm->tm_min
		, current_tm->tm_sec
		, value_in_actual_unit(interface->stat.in_bytes, s_unit_set)
		, value_in_actual_unit(interface->stat.out_bytes, s_unit_set)
		, "120"
		, "120"
		);

	fflush(stdout);

	return 0;
}

void cleanup_stdout()
{
	time_t session_duration;
	unsigned long act_tx;
	unsigned long act_rx;
	unsigned long avg_tx_speed;
	unsigned long avg_rx_speed;

	end_time = time(NULL);
	session_duration = difftime(end_time, start_time);

	act_tx = value_in_actual_unit(total_data_tx, s_unit_set);
	act_rx = value_in_actual_unit(total_data_rx, s_unit_set);
	avg_tx_speed = act_tx - value_in_actual_unit(start_data_tx, s_unit_set) / session_duration;
	avg_rx_speed = act_rx - value_in_actual_unit(start_data_rx, s_unit_set) / session_duration; 

	printf("\n+---------------------+-----------------+--------------------+---------------------+---------------------+\n");
	printf("\n * Total statistics for interface:");
	printf("\n\t Transmitted %s: %ld", data_amounts[s_unit_set], act_tx);
	printf("\n\t Received %s: %ld", data_amounts[s_unit_set], act_rx);
	printf("\n\t Session duration in seconds: %u", (unsigned)session_duration);
	printf("\n\t Average download speed during session: %ld %s", avg_rx_speed, speed_values[s_unit_set]);
	printf("\n\t Average upload speed during session: %ld %s", avg_tx_speed, speed_values[s_unit_set]);
	printf("\n\n * Other information:");
	printf("\n\t Transmitted packets: %u", total_packets_tx);
	printf("\n\t Received packets: %u", total_packets_rx);
	printf("\n\t Dropped input packets: %u", total_dropped_packets_rx);
	printf("\n\t Dropped output packets: %u", total_dropped_packets_tx);
	printf("\n\t Transmit errors: %u", total_errors_tx);
	printf("\n----\n");

	fflush(stdout);
}

