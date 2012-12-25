/* linux/intefaces.c
    - implementation of all interface specific methods

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

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "interfaces.h"
#include "sysfs_reader.h"

void dump_interfaces()
{
	int sock;
	int ifNum;
	int iCount;
	struct ifreq* ifr;
	struct ifconf ifc;
	struct sockaddr_in* sin;
	char strIp4Addr[INET_ADDRSTRLEN] = {0};
	char strIp6Addr[INET6_ADDRSTRLEN] = {0};

	memset(&ifc, 0, sizeof(ifc));

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		return;
	}

	if (ioctl(sock, SIOCGIFCONF, &ifc) < 0) {
     		perror("ioctl");
		close(sock);
		return;
	}

	if ((ifr = (struct ifreq*)malloc(ifc.ifc_len)) == NULL) {
     		perror("malloc");
		close(sock);
     		return;
   	}

	ifc.ifc_ifcu.ifcu_req = ifr;

	if (ioctl(sock, SIOCGIFCONF, &ifc) < 0) {
		perror("ioctl");
		close(sock);
		free(ifr);
		return;
	}

	close(sock);

	ifNum = ifc.ifc_len / sizeof(struct ifreq);

	if (!ifNum) {
		free(ifr);
		return;
	}

	for (iCount = 0; iCount < ifNum; iCount++) {
		if (strcmp(ifr[iCount].ifr_name, "lo") == 0) {
			continue;
		}

		sin = (struct sockaddr_in *)&ifr[iCount].ifr_addr;

		inet_ntop(AF_INET, &sin->sin_addr, strIp4Addr, INET_ADDRSTRLEN);

		printf("%i: %s:\n", ifr[iCount].ifr_ifindex, ifr[iCount].ifr_name);
		printf("\tinet: %s\n\tinet6: %s\n\n", strIp4Addr, strIp6Addr);
	}

	free(ifr);
}


static int get_interface_ipv4_info(net_iface_t* interface)
{
	
}

static int get_interface_ipv6_info(net_iface_t* interface)
{
	
}

static int get_interface_info(net_iface_t* interface)
{
	return 0;
}

int setup_interface(net_iface_t* interface)
{
	if (!is_sysfs_available()) {
		printf("/sysfs is not available on your system!\n");
		return -1;
	}

	interface->get_stat = get_stat_sysfs;

	return get_interface_info(interface);
}

