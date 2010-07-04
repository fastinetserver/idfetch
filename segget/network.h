/*
* Copyright (C) 2010 Robin H.Johnson, Ovechko Kostyantyn <fastinetserver@gmail.com>.
*
* Project: IDFetch.
* Developer: Ovechko Kostyantyn Olexandrovich (Kharkiv State Technical University of Construction and Architecture, Ukraine).
* Mentor: Robin H. Johnson (Gentoo Linux: Developer, Trustee & Infrastructure Lead).
* Mentoring organization: Gentoo Linux.
* Sponsored by GSOC 2010.
*
* This file is part of Segget.
*
* Segget is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* Segget is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with Segget; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/
#ifndef __NETWORK_H__
#define __NETWORK_H__

//#include <string>
//#include <map>
#include "str.h"
#include <vector>
#include "mirror.h"
#include "config.h"
//#include "tui.h"
//#include "settings.cpp"
using namespace std;

#define MAX_NETWORKS 10

class Tnetwork{
	static uint network_count;
	private:
		void load_mirror_list();
	public:
		vector<string> mirror_list;
		vector<Tmirror> benchmarked_mirror_list;
		uint active_connections_num;
	//network
		uint network_num;
		uint priority;
	//connections
		ulong max_connections;
		ulong connection_timeout;
		ulong ftp_response_timeout;
		ulong time_out;
		ulong low_connection_speed_limit;
		ulong low_connection_speed_time;
		ulong max_connection_speed;
		ulong current_speed_time_interval_msecs;
		string bind_interface;
	//user-data
		string user_agent;
	//proxy
		string proxy_ip_or_name;
		ulong proxy_port;
		bool proxy_off;
		string proxy_user;
		string proxy_password;
	//mirrors
		bool use_own_mirror_list_only_on;
		bool only_local_when_possible;
		Tnetwork():
			mirror_list(),
			benchmarked_mirror_list(),
			active_connections_num(0),
		//network
			network_num(network_count),
			priority(0),
		//connections
			max_connections(6),
			connection_timeout(15),
			ftp_response_timeout(180),
			time_out(500),
			low_connection_speed_limit(1000),
			low_connection_speed_time(10),
			max_connection_speed(0),
			current_speed_time_interval_msecs(1000),
			bind_interface("none"),
		//user-data
			user_agent("segget"),
		//proxy
			proxy_ip_or_name("none"),
			proxy_port(3128),
			proxy_off(1),
			proxy_user("none"),
			proxy_password("none"),
		//mirrors
			use_own_mirror_list_only_on(0),
			only_local_when_possible(1)
			{network_count++;};
		void init(uint priority_value);
		bool has_free_connections();
		bool connect();
		void disconnect(){active_connections_num--;};
};

uint Tnetwork::network_count=0;
Tnetwork network_array[MAX_NETWORKS];
#endif