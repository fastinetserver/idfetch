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

#ifndef __SCRIPTSERVER_H__
#define __SCRIPTSERVER_H__

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <stdio.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <string.h>
#include <json/json.h>
#include "tui.h"
#include "pkg.h"

enum Tvar_nums{
	CONNECTION_NUM,
	CONNECTION_URL,
	CONNECTION_MAX_SPEED_LIMIT,
	NETWORK_NUM,
	NETWORK_MODE,
	NETWORK_ACTIVE_CONNECTIONS_COUNT,
	DISTFILE_NAME,
	DISTFILE_SIZE,
	DISTFILE_DLD_SEGMENTS_COUNT,
	DISTFILE_SEGMENTS_COUNT,
	DISTFILE_ACTIVE_CONNECTIONS_COUNT,
	SEGMENT_NUM,
	SEGMENT_TRY_NUM,
	SEGMENT_SIZE,
	SEGMENT_RANGE
};

extern map<std::string, Tvar_nums> variables_;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

//void *run_script_server(void * );
bool run_user_python_script(uint connection_num);
#endif