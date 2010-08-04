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

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <ncurses.h>
#include <sys/types.h>
#include "twindow.h"
#include "helpwindow.h"
#include "distfilewindow.h"
#include "scrollwindow.h"

using namespace std;

const uint CONNECTION_LINES=5;
const uint MAX_LINES=200;

class Tmainwindow: public Tscroll_window{
	public:
		
		Thelp_window help_win;
		Tscroll_window log_win;
		Tscroll_window error_log_win;
		Tdistfile_window distfiles_win;
//		bool exit_flag;
		bool connected_status;
//		string screenlines[200];
		string screen_info_lines[4];
		ulong attempt_num;
		bool disconnected_flag;
		Tmainwindow():
			help_win(),
			log_win(),
			error_log_win(),
			distfiles_win(),
//			exit_flag(FALSE),
			connected_status(FALSE),
			attempt_num(0),
			disconnected_flag(true)
			{};
		~Tmainwindow(){};
		Tmainwindow(const Twindow &L); // copy constructor
		Tmainwindow & operator=(const Twindow &L);
		void msg_status();
		void compose();
		void set_status(string str);
		void set_line(uint y, string msg_text);
		void init();
		void connected();
		void disconnected();
};
#endif