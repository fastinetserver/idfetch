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

void Tnetwork::init(uint priority_value){
	try{
		priority=priority_value;
		ConfigFile conf("network"+toString(network_num)+".conf");
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
	}
	catch(...)
	{
		error_log_no_msg("Error calling msg() in settings.cpp: load_from_conf_file()");
	}
}

bool Tnetwork::get_busy_status(){
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
