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
//#include "tui.cpp"
#define MAX_CONNECTS 20 /* number of simultaneous transfers */

using namespace std;

class Tsettings{
	public:
		string conf_dir;
	//pkg_list
		string pkg_list_file_name;
		bool keep_pkg_list_file;
	//request_server
		string request_ip;
		ulong request_port;
	//logs
//		string logs_dir;
//		string general_log_file;
//		string error_log_file;
//		string debug_log_file;

	Tsettings():
			conf_dir("/etc/seggetd"),
		//pkg_list
			pkg_list_file_name("/var/tmp/seggetd/pkg.list"),
			keep_pkg_list_file(false),
		//request_server
			request_ip("127.0.0.1"),
			request_port(10000)
		//logs
//			logs_dir("./logs"),
//			general_log_file("segget.log"),
//			error_log_file("error.log"),
//			debug_log_file("debug.log")
			{};
		void init();
};

extern Tsettings settings;
#endif