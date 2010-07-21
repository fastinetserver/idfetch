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
#include "settings.h"
#include "ui_server.h"
#include "utils.h"

using namespace std;

void msg_total(string msg_text);
void msg(uint y, uint x, string msg_text);
void msg_connecting(uint connection_num, uint distfile_num, uint segment_num, string msg_text);
void msg_segment_progress(uint connection_num, uint network_num, uint segment_num, uint try_num, ulong dld_bytes, ulong total_bytes, ulong speed, ulong avg_speed);
void msg_status1(uint connection_num, uint segment_num, string msg_text);
void msg_status2(uint connection_num, string msg_text);
void msg_clean_connection(uint connection_num);
void msg_error(string error_text);
void msg_total(string msg_text);

void log(string log_msg_text);
void debug_no_msg(string debug_msg_text);
void debug(string debug_msg_text);
void error_log(string error_msg_text);
void error_log_no_msg(string error_msg_text);
#endif