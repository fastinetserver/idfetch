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
#include <signal.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <json/json.h>
#include <ncurses.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "checksum.h"
#include "config.h"
#include "distfile.h"
#include "mirror.h"
#include "network.h"
#include "networkbroker.h"
#include "pkg.h"
#include "segment.h"
#include "settings.h"
//#include "stats.h"
#include "str.h"
#include "tui.h"
#include "utils.h"
#include "ui_server.h"
#include "proxyfetcher.h"
#include "requestserver.h"
#include "scriptserver.h"

using namespace std;

CURLM *cm;

int routine();
void start_daemon_mode();
int parse_cli_arguments(int argc, char* argv[]);
int init_curses();
//int load_pkgs();
void show_pkgs();
int pkg_choose_segment(Tpkg * cur_pkg, uint connection_num);
int choose_segment(uint connection_num);
int download_pkgs();
int main(int argc, char* argv[]);
void *refresh_tui_screen( void *);
void launch_tui_thread();
void launch_ui_server_thread();
void launch_proxy_fetcher_server_thread();
void launch_request_server_thread();

#endif