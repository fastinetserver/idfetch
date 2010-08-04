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

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <ncurses.h>
#include <pthread.h>
#include "str.h"
#include "twindow.h"
#include "mainwindow.h"
#include "helpwindow.h"
#include "scrollwindow.h"
#include "colors.h"
#include "config.h"
#include "log.h"
#include "settings.h"
#include "tuidistfile.h"
#include "distfilewindow.h"

#define BUFFER_SIZE 2000

#define TOTAL_LINE_NUM	200
#define ERROR_LINE_NUM	201
#define LOG_LINE_NUM	202
#define DEBUG_LINE_NUM	203

using namespace std;

bool exit_flag=false;
bool thread_exited=false;
string exit_msg="";
int exit_code=0;
int watch_keyboard_thread_status=10;
pthread_t watch_keyboard_thread;
Tmainwindow mainwindow;

int cur_window_num=0;
int sockfd;
void exit_curses_and_show_msg();
void quit(int exit_code, string error_msg);
void * watch_keyboard_thread_function(void * ptr);
void * refresh_screen_thread_function(void * ptr);