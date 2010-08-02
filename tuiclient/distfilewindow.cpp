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

#include "distfilewindow.h"

void Tdistfile_window::compose(){
	color_default_window(window);
	make_frame();
//	box(window, ACS_VLINE, ACS_HLINE);
//	getmaxyx(window,height,width);
	bottom_screenline_num=height-2;
//	color_status();
//	msg_status();
//	color_info();
//	for (uint y=bottom_screenline_num,status_line_num=0; y<height-2; y++,status_line_num++){
//		msg_line(y+1,screen_info_lines[status_line_num]);
//	}
//	color_downloads(window);
//	screenlines[26]="Max_num:"+toString(max_received_screenline_num);
	ulong distfile_num=top_position;
	for (int y=0; y<bottom_screenline_num and distfile_num<tuidistfiles.size(); y++, distfile_num++){
		msg_line(y+1,field("",distfile_num+1,4)+") "
			+"("+toString(tuidistfiles[distfile_num].dld_bytes*100/tuidistfiles[distfile_num].size)+"%) "
			+tuidistfiles[distfile_num].name
			+" Segments: "+toString(tuidistfiles[distfile_num].dld_segments)
			+"/"+toString(tuidistfiles[distfile_num].segments_count)
			+" Bytes: "+toString(tuidistfiles[distfile_num].dld_bytes)
			+"/"+toString(tuidistfiles[distfile_num].size));
	}
	wrefresh(window);
//	mainwindow.distfiles_win.add_line(parts[0]+"("+toString(atol(parts[3].c_str())*100/)+"%)"+" "+parts[1]+"/"+parts[2]+" "+parts[3]+"/"+parts[4]);
}



