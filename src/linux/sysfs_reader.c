/* linux/sysfs_reader.c
    - implementation of linux statistic reader using sysfs

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
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "interfaces.h"
#include "sysfs_reader.h"

#define SYSFS_PATH "/sys/class/net/"
#define STAT_DIR "/statistics/"
#define RX_BYTES_FILE "rx_bytes"
#define TX_BYTES_FILE "tx_bytes"
#define RX_PACKETS_FILE "rx_packets"
#define TX_PACKETS_FILE "tx_packets"
#define RX_DROPPED_FILE "rx_dropped"
#define TX_DROPPED_FILE "tx_dropped"
#define TX_ERRORS "tx_errors"

int is_sysfs_available()
{
	struct stat file_stat;
	return stat(SYSFS_PATH, &file_stat) != -1;
}

char* create_path(const char* dir, const char* ifname, const char* stdir, const char* file)
{
	if (!dir || !ifname || !stdir || !file) {
		return NULL;
	}

	char* path;

	size_t total_size = strlen(dir) + strlen(ifname) + strlen(stdir) + strlen(file);

	if ((path = (char*)malloc(sizeof(char) * total_size + 2)) == NULL) {
		perror("malloc");
		return NULL;
	}

	strcpy(path, dir);
	strcat(path, ifname);
	strcat(path, stdir);
	strcat(path, file);

	path[total_size] = '\0';

	return path;
}

long long int get_data_from_file(const char* ifname, const char* file)
{
	char* path = create_path(SYSFS_PATH, ifname, STAT_DIR, file);

	if (!path) {
		return -1;
	}

	FILE* s_file = fopen(path, "r");

	if (!s_file) {
		free(path);
		return -1;
	}

	char file_content[32];
	size_t file_content_length;

	file_content_length = fread(file_content, sizeof(file_content[0])
					, sizeof(file_content) / sizeof(file_content[0]), s_file);
	fclose(s_file);
	free(path);

	if (!file_content_length) {
		return -1;
	}

	return strtoul(file_content, 0, 10);
}

int get_stat_sysfs(if_stat_t* stat, const char* ifname)
{
	if (!stat) {
		return -1;
	}

	stat->in_bytes = get_data_from_file(ifname, RX_BYTES_FILE);
	stat->out_bytes = get_data_from_file(ifname, TX_BYTES_FILE);
	stat->in_packets = get_data_from_file(ifname, RX_PACKETS_FILE);
	stat->out_packets = get_data_from_file(ifname, TX_PACKETS_FILE);

	if ( stat->in_bytes == -1 || stat->out_bytes == -1 
	     || stat->in_packets == -1 || stat->out_packets == -1 ) {
		return -1;
	}

	stat->in_dropped = get_data_from_file(ifname, RX_DROPPED_FILE);
	stat->out_dropped = get_data_from_file(ifname, TX_DROPPED_FILE);
	stat->if_errors = get_data_from_file(ifname, TX_ERRORS);

	return 0;
}


