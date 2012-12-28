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

static float last_data_rx_probe = 0;
static float last_data_tx_probe = 0;

static unsigned int total_packets_tx = 0;
static unsigned int total_packets_rx = 0;

static unsigned int total_dropped_packets_rx = 0;
static unsigned int total_dropped_packets_tx = 0;
static unsigned int total_errors_tx = 0;

static time_t start_time = 0;
static time_t end_time = 0;

static int start_from_zero = 0;

void print_header(const speed_unit_t s_unit)
{
	printf("+---------------------+-----------------+--------------------+---------------------+----------------------+\n");
	printf("| Date/time           | Received %-6s | Transmitted %-6s | In speed %-10s | Out speed %-10s |\n",
			data_amounts[s_unit], data_amounts[s_unit], speed_values[s_unit], speed_values[s_unit]);
	printf("+---------------------+-----------------+--------------------+---------------------+----------------------+\n");
	fflush(stdout);
}

int init_stdout_dump(const int zero, const speed_unit_t s_unit)
{
	start_from_zero = zero;
	s_unit_set = s_unit;
	print_header(s_unit);
	return 0;
}

int dump_stat_stdout(net_iface_t* interface)
{
	time_t time_now = time(NULL);
	float actual_data_tx = 0;
	float actual_data_rx = 0;
	float show_data_tx = 0;
	float show_data_rx = 0;

	float speed_value_tx = 0;
	float speed_value_rx = 0;

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

	actual_data_tx = (float)interface->stat.out_bytes;
	actual_data_rx = (float)interface->stat.in_bytes;

	show_data_tx = start_from_zero ? actual_data_tx - (float) start_data_tx : actual_data_tx;
	show_data_rx = start_from_zero ? actual_data_rx - (float) start_data_rx : actual_data_rx;

	speed_value_rx = last_data_rx_probe ? last_data_rx_probe - show_data_rx / 1000 : 0;
	speed_value_tx = last_data_tx_probe ? last_data_tx_probe - show_data_tx / 1000 : 0;

	printf("| %04i-%02i-%02i/%02i:%02i:%02i | %-15.2f | %-18.2f | %-19.2lf | %-20.2lf |\r"
		, current_tm->tm_year + START_CALENDAR_DATE
		, current_tm->tm_mon + MONTH_OFFSET
		, current_tm->tm_mday
		, current_tm->tm_hour
		, current_tm->tm_min
		, current_tm->tm_sec
		, value_in_actual_unit(show_data_rx, s_unit_set)
		, value_in_actual_unit(show_data_tx, s_unit_set)
		, value_in_actual_unit(speed_value_rx, s_unit_set)
		, value_in_actual_unit(speed_value_tx, s_unit_set)
		);

	fflush(stdout);

	last_data_rx_probe = show_data_tx;
	last_data_tx_probe = show_data_rx;

	total_data_rx += interface->stat.in_bytes;
	total_data_tx += interface->stat.out_bytes;

	total_packets_tx += interface->stat.out_packets;
	total_packets_rx += interface->stat.in_packets;
	total_dropped_packets_rx += interface->stat.in_dropped;
	total_dropped_packets_tx += interface->stat.out_dropped;
	total_errors_tx += interface->stat.if_errors;

	return 0;
}

void cleanup_stdout()
{
	time_t session_duration;
	float act_tx;
	float act_rx;
	float avg_tx_speed;
	float avg_rx_speed;

	end_time = time(NULL);
	session_duration = difftime(end_time, start_time);

	act_tx = value_in_actual_unit(total_data_tx, s_unit_set);
	act_rx = value_in_actual_unit(total_data_rx, s_unit_set);
	avg_tx_speed = act_tx - value_in_actual_unit(start_data_tx, s_unit_set) / session_duration;
	avg_rx_speed = act_rx - value_in_actual_unit(start_data_rx, s_unit_set) / session_duration; 

	printf("\n+---------------------+-----------------+--------------------+---------------------+----------------------+\n");
	printf("\n * Total statistics for interface:");
	printf("\n\t Transmitted %s: %12.2lf", data_amounts[s_unit_set], act_tx);
	printf("\n\t Received %s: %12.2lf", data_amounts[s_unit_set], act_rx);
	printf("\n\t Session duration in seconds: %u", (unsigned)session_duration);
	printf("\n\t Average download speed during session: %12.2lf %s", avg_rx_speed, speed_values[s_unit_set]);
	printf("\n\t Average upload speed during session: %12.2lf %s", avg_tx_speed, speed_values[s_unit_set]);
	printf("\n\n * Other information:");
	printf("\n\t Transmitted packets: %u", total_packets_tx);
	printf("\n\t Received packets: %u", total_packets_rx);
	printf("\n\t Dropped input packets: %u", total_dropped_packets_rx);
	printf("\n\t Dropped output packets: %u", total_dropped_packets_tx);
	printf("\n\t Transmit errors: %u", total_errors_tx);
	printf("\n----\n");

	fflush(stdout);
}

