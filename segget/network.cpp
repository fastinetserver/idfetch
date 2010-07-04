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
#include "network.h"
void Tnetwork::load_mirror_list(){
	ifstream file;
	string mirror_list_file_name="network"+toString(network_num)+"_mirrors.conf";
	file.exceptions (ifstream::failbit | ifstream::badbit);
	try{
		file.open(mirror_list_file_name.c_str());
	}
	catch(...){
		error_log("Can NOT open mirror list file: "+mirror_list_file_name+". Network will be disabled");
		priority=0;
		return;
	}
	try{
		//processing file
		string mirror_line;
		while (not(file.eof())) {
			getline(file,mirror_line);
			if (! mirror_line.length()) continue;
			if (mirror_line[0] == '#') continue;
			if (mirror_line[0] == ';') continue;
			Tmirror cur_mirror;
			cur_mirror.url=mirror_line;
			benchmarked_mirror_list.push_back(cur_mirror);
			debug("LOCAL_MIRROR_ADDED:"+mirror_line);
		}
	}
	catch(...){
		error_log("Mirror list file: "+mirror_list_file_name+" was opened, but an error occured while reading from it.");
	}
}

void Tnetwork::init(uint priority_value){
	try{
		priority=priority_value;
		Tconfig conf("network"+toString(network_num)+".conf");
		conf.set(bind_interface,					"network_bind",		"bind_interface");
		conf.set(max_connections,					"network_connections",		"max_connections",1,MAX_CONNECTS);
		conf.set(connection_timeout,				"network_connections",		"connection_timeout",1,1000);
		conf.set(ftp_response_timeout,				"network_connections",		"ftp_response_timeout",1,-1);
		conf.set(time_out,							"network_connections",		"timeout",100,-1);
		conf.set(low_connection_speed_limit,		"network_connections",		"low_connection_speed_limit",1,-1);
		conf.set(low_connection_speed_time,			"network_connections",		"low_connection_speed_time",1,600);
		conf.set(max_connection_speed,				"network_connections",		"max_connection_speed",1,-1);

		conf.set(user_agent,						"network_user_data",		"user_agent");

		conf.set(proxy_ip_or_name,					"network_proxy",			"proxy_ip_or_name");
		conf.set(proxy_port,						"network_proxy",			"proxy_port",1,65535);
		conf.set(proxy_off,							"network_proxy",			"proxy_off");
		conf.set(proxy_user,						"network_proxy",			"proxy_user");
		conf.set(proxy_password,					"network_proxy",			"proxy_password");

		conf.set(use_own_mirror_list_only_on,		"network_mirrors",			"use_own_mirror_list_only_on");

		if (use_own_mirror_list_only_on){
			conf.set(only_local_when_possible,			"network_mirrors",			"only_local_when_possible");
			load_mirror_list();
			log("Settings: Network"+toString(network_num)+" local mirror_list size:"+toString(mirror_list.size()));
		}
		conf.clear();
	}
	catch(...)
	{
		error_log_no_msg("Error calling msg() in settings.cpp: load_from_conf_file()");
	}
}

bool Tnetwork::has_free_connections(){
	if (active_connections_num<max_connections){
		return true;
	}else{
		return false;
	}
}

bool Tnetwork::connect(){
	if (active_connections_num<max_connections){
		active_connections_num++;
		return true;
	}else{
		return false;
	}
}