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

#ifndef _TWINDOW_H_
#define _TWINDOW_H_

#include <ncurses.h>
#include <sys/types.h>
#include "colors.h"
//#include "tuiclient.h"

using namespace std;

class Twindow{
	public:
		WINDOW * window;
		string caption;
		uint width;
		uint height;
		uint y;
		uint x;
		bool notfresh;
		bool visible;
		int top_position;
		int bottom_screenline_num;
		int max_received_screenline_num;
		string status_str;

		Twindow():
			window(0),
			caption(""),
			width(10),
			height(10),
			y(1),
			x(1),
			notfresh(FALSE),
			visible(FALSE),
			top_position(0),
			bottom_screenline_num(0),
			max_received_screenline_num(0),
			status_str("")
			{};
		virtual ~Twindow(){};
		Twindow(const Twindow &L); // copy constructor
		Twindow & operator=(const Twindow &L);
		void init(string caption_, int height_, int width_, int y_, int x_);
		void toggle();
		void msg_line(uint y, string msg_text);
		void msg_short(uint y, uint x, string msg_text);
		void up(int inc);
		void down(uint inc);
		void center(uint max_y, uint max_x);
		void resize(uint new_height, uint new_width, uint new_y, uint new_x);
		void make_frame();
		void refresh();
		void show();
		virtual void compose(){};
};

extern string sms;
#endif