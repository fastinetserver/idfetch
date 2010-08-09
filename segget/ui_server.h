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

#ifndef __UI_SERVER_H__
#define __UI_SERVER_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
//#include "settings.h"
//class Tpkg;
//class Tdistfile;
//#include "tui.h"
#include "pkg.h"
using namespace std;

#define MAX_BIND_ATTEMPTS 5

//const uint MAX_LINES=200;

class Tui_server{
	public:
		bool send_to_fd_busy;
		uint server_sockfd;
		uint max_fd_num;
		fd_set readfds, tuiclient_fds_set, testfds;
		void init();
		ulong send_to_fd(uint fd, string msg);
		void send_connection_msg_to_fd(uint fd, uint y, string msg);
		void send_connection_msg_to_all_clients(uint y, string msg);
		void send_log_msg_to_all_clients(string msg);
		void send_error_log_msg_to_all_clients(string msg);
		void send_distfile_progress_msg_to_fd(uint fd, string msg);
		void send_distfile_progress_msg_to_all_clients(string msg);
		void serve_tuiclient(uint fd, string msg);
		ulong send_binary_to_fd(uint fd, string image_file_name);
		string serve_browser_distfile_progress(Tdistfile * a_distfile);
		string get_html_header(string title);
		string get_html_footer();
		string get_html_connections();
		string get_html_distfiles();
		string get_html_stats();
		string get_html_log();
		string get_html_errors_log();
		string get_html_mirrors_stats();
		string get_ajax_for(string content_name);
		string get_rss_info();
		void serve_browser(uint fd, string msg);
};

extern Tui_server ui_server;

void *run_ui_server(void *);

#endif