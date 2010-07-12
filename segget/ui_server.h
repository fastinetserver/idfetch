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
#include <string.h>

using namespace std;

#define MAX_BIND_ATTEMPTS 5
const uint MAX_LINES=200;
const uint TOTAL_LINE_NUM=MAX_LINES;
const uint ERROR_LINE_NUM=MAX_LINES+1;
const uint LOG_LINE_NUM=MAX_LINES+2;
const uint DEBUG_LINE_NUM=MAX_LINES+3;

//const uint MAX_LINES=200;
string screenlines[DEBUG_LINE_NUM+1];
uint max_published_screenline_num;

class Tui_server{
	public:
		bool send_to_fd_busy;
		uint server_sockfd;
		uint max_fd_num;
		fd_set readfds, testfds;
		void init();
		ulong send_to_fd(uint fd, uint y, string msg);
		void send_all_clients(uint y, string msg);
};

Tui_server ui_server;

void *run_ui_server(void * ptr);

#endif