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

#include "settings.h"

Tsettings settings;

void Tsettings::load_from_conf_file(){
	try{
		ConfigFile conf("segget.conf");
		conf.set(general_log_file,					"logs",				"general_log_file");
		conf.set(logs_dir,							"logs",				"logs_dir");
		conf.set(error_log_file,					"logs",				"error_log_file");
		conf.set(debug_log_file,					"logs",				"debug_log_file");

		conf.set(distfiles_dir,						"folders",			"distfiles_dir");
		conf.set(segments_dir,						"folders",			"segments_dir");
	//	log("segments_dir set to:"+segments_dir);

		conf.set(max_connection_num_per_distfile,	"distfiles",		"max_connection_num_per_distfile",1,20);

		conf.set(resume_on,							"segments",			"resume_on");
		conf.set(max_segment_size,					"segments",			"max_segment_size",10000,10000000);
		conf.set(max_tries,							"segments",			"max_tries",1,-1);

		conf.set(max_connections,					"connections",		"max_connections",1,20);
		conf.set(connection_timeout,				"connections",		"connection_timeout",1,1000);
		conf.set(ftp_response_timeout,				"connections",		"ftp_response_timeout",1,-1);
		conf.set(time_out,							"connections",		"timeout",100,-1);
		conf.set(low_connection_speed_limit,		"connections",		"low_connection_speed_limit",0,-1);
		conf.set(low_connection_speed_time,			"connections",		"low_connection_speed_time",1,600);
		conf.set(max_connection_speed,				"connections",		"max_connection_speed",0,-1);
		conf.set(current_speed_time_interval_msecs,	"connections",		"current_speed_time_interval_msecs",100,60000);
		conf.set(bind_interface,					"connections",		"bind_interface");

		conf.set(max_connections_num_per_mirror,	"mirrors",			"max_connections_num_per_mirror",1,10);
		conf.set(benchmark_oblivion,				"mirrors",			"benchmark_oblivion",1,1000);

		conf.set(user_agent,						"user-data",		"user_agent");

		conf.set(proxy_ip_or_name,					"proxy",			"proxy_ip_or_name");
		conf.set(proxy_port,						"proxy",			"proxy_port",1,65535);
		conf.set(proxy_off,							"proxy",			"proxy_off");
		conf.set(proxy_user,						"proxy",			"proxy_user");
		conf.set(proxy_password,					"proxy",			"proxy_password");
	}
	catch(...)
	{
		error_log_no_msg("Error calling msg() in settings.cpp: load_from_conf_file()");
	}

}