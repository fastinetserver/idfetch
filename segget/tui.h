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

#ifndef __TUI_H__
#define __TUI_H__
#include <ncurses.h>
#include <vector>
#include "settings.h"
#include "ui_server.h"
#include "utils.h"
#include "log.h"

using namespace std;

const uint MAX_LINES=200;
const uint TOTAL_LINE_NUM=MAX_LINES;
const uint ERROR_LINE_NUM=MAX_LINES+1;
const uint LOG_LINE_NUM=MAX_LINES+2;
const uint DEBUG_LINE_NUM=MAX_LINES+3;
const uint LOG_LINES_MAX_NUM=200;

extern string screenlines[DEBUG_LINE_NUM+1];
extern uint max_published_screenline_num;

extern vector<string> log_lines;
extern uint log_lines_counter;

void msg_total(string msg_text);
void msg(uint y, uint x, string msg_text);
void msg_connecting(uint connection_num, uint distfile_num, uint segment_num, string msg_text);
void msg_segment_progress(uint connection_num, string progress_text);
void msg_status1(uint connection_num, uint segment_num, string msg_text);
void msg_status2(uint connection_num, string msg_text);
void msg_clean_connection(uint connection_num);
void msg_log(string log_text);
void msg_error_log(string error_log_text);
void msg_total(string msg_text);
#endif