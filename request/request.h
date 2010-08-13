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

#ifndef __SEGGET_H__
#define __SEGGET_H__

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <json/json.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/socket.h>
//#include <stdio.h>
//#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
//#include <unistd.h>
//#include <stdlib.h>
//#include <string>
//#include <string.h>


#include "config.h"
#include "settings.h"
//#include "stats.h"
#include "str.h"
#include "utils.h"
#include "response.h"

using namespace std;
int sockfd;

int routine();
int parse_cli_arguments(int argc, char* argv[]);
int load_pkgs();
//void show_pkgs();
int main(int argc, char* argv[]);

#endif