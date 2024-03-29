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

#ifndef __LOG_H__
#define __LOG_H__
#include "settings.h"
#include "utils.h"
#define LOG_LINES_MAX_NUM 200

using namespace std;
extern vector<string> rss_distfile_lines;
extern vector<ulong> rss_size_lines;
extern vector<string> rss_time_lines;
extern vector<string> log_lines;
extern vector<string> error_log_lines;

void rss_log(string distfile_name, ulong distfile_size);
void log_no_msg(string log_msg_text);
void log(string log_msg_text);
void debug_no_msg(string debug_msg_text);
void debug(string debug_msg_text);
void error_log_no_msg(string error_msg_text);
void error_log(string error_msg_text);
string get_time(string time_format);
#endif