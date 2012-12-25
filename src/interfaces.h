/* interfaces.h
    - declaration of all interface specific methods

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

#ifndef INTERFACES_H
#define INTERFACES_H

#include <sys/socket.h>
#if HAVE_LINUX
#include <linux/if.h>
#endif
#include <netinet/in.h>
#include <net/if.h>

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 48
#endif

struct if_stat {
	long long in_bytes;
	long long in_packets;
	long long out_bytes;
	long long out_packets;
	long long in_dropped;
	long long out_dropped;
	long long if_errors;
};
typedef struct if_stat if_stat_t;

struct net_iface {
	int if_idx;
	char if_name[IFNAMSIZ];
	if_stat_t stat;
	char str_ip4_addr[INET_ADDRSTRLEN];
	char str_ip6_addr[INET6_ADDRSTRLEN];
	int (*get_stat) (if_stat_t*, const char*);
};
typedef struct net_iface net_iface_t;


int setup_interface(net_iface_t* interface);

void dump_interfaces();

#endif

