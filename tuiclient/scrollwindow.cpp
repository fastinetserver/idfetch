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

#include "scrollwindow.h"

//Tscroll_window::~Tscroll_window(){};

void Tscroll_window::compose(){
//		msg_text=msg_text+" ";
//	
	getmaxyx(window,height,width);
	bottom_screenline_num=height-2;
	color_scroll_window(window);
	make_frame();
	color_downloads(window);
//	screenlines[26]="Max_num:"+toString(max_received_screenline_num);
	top_position=scroll_lines.size()-height+2; // +2 for border lines
//	if (top_position<0) {top_position=0;};
	uint line_num=top_position;
	for (uint y=0; (y<bottom_screenline_num && line_num<scroll_lines.size()); y++, line_num++){
		msg_line(y+1,scroll_lines[line_num]);
	}
	wrefresh(window);
}


void Tscroll_window::add_line(string line){
		max_received_screenline_num=scroll_lines.size();
		scroll_lines.push_back(line);
		if (scroll_lines.size()>SCROLL_LINES_MAX_NUM){
			scroll_lines.erase(scroll_lines.begin(),scroll_lines.begin()+scroll_lines.size()-SCROLL_LINES_MAX_NUM);
		}
}