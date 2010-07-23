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

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <string>
#include "config.h"
#include "network.h"
//#include "tui.cpp"
#define MAX_CONNECTS 20 /* number of simultaneous transfers */

using namespace std;

class Tsettings{
	private:
		void load_provide_mirror_files_restricted_patterns_vector();
	public:
		string conf_dir;
		bool no_daemon_flag;
	//folders
		string distfiles_dir;
		string segments_dir;
	//pkg_list
		string pkg_list_dir;
		bool del_pkg_list_when_dld_finished;
	//distfiles
		ulong max_connection_num_per_distfile;
	//segments
		ulong max_segment_size;
		bool resume_on;
		ulong max_tries;
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
	//mirrors
		ulong max_connections_num_per_mirror;
		ulong benchmark_oblivion;
	//provide_mirror_to_others
		string provide_mirror_dir;
		bool provide_mirror_files_restrict_list_on;
		vector<string> provide_mirror_files_restricted_patterns_vector;
	//provide_proxy_fether_to_others
		string provide_proxy_fetcher_ip;
		ulong provide_proxy_fetcher_port;
	//ui_server
		string ui_ip;
		ulong ui_port;
	//logs
		string logs_dir;
		string general_log_file;
		string error_log_file;
		string debug_log_file;

	Tsettings():
			conf_dir("/etc/seggetd"),
			no_daemon_flag(false),
		//folders
			distfiles_dir("./distfiles"),
			segments_dir("./tmp"),
		//pkg_list
			pkg_list_dir("./"),
			del_pkg_list_when_dld_finished(1),
		//distfiles
			max_connection_num_per_distfile(3),
		//segments
			max_segment_size(500000),
			resume_on(1),
			max_tries(10),
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
		//mirrors
			max_connections_num_per_mirror(1),
			benchmark_oblivion(5),
		//provide_mirror_to_others
			provide_mirror_dir("none"),
			provide_mirror_files_restrict_list_on(0),
			provide_mirror_files_restricted_patterns_vector(),
		//provide_proxy_fether_to_others
			provide_proxy_fetcher_ip("127.0.0.1"),
			provide_proxy_fetcher_port(9888),
		//ui_server
			ui_ip("127.0.0.1"),
			ui_port(9999),
		//logs
			logs_dir("./logs"),
			general_log_file("segget.log"),
			error_log_file("error.log"),
			debug_log_file("debug.log")
			{};
		void set_resume(bool resume_setting){resume_on=resume_setting;};
		bool get_resume(){return resume_on;};
		void init();
};

extern Tsettings settings;
#endif