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

void Tdistfile_window::make_frame(){
	wclear(window);
	box(window, ACS_VLINE, ACS_HLINE);
	mvwaddstr(window,0,(width-caption.length())/2,caption.c_str());
//	msg_short(0,width-20,"[Lines:"+toString(top_position+1)+"-"+toString(top_position+bottom_screenline_num)+"/"+toString(max_received_screenline_num)+"]");
	msg_short(0,width-20,"[Lines:"+toString(top_position+1)+"-"+toString(top_position+bottom_screenline_num)+"/"+toString(tuidistfiles.size())+"]");
	msg_short(height-1,width-27,"[Up/Dn/PgUp/PgDn - scroll]");
}

void Tdistfile_window::compose(){
	max_received_screenline_num=tuidistfiles.size();
//	color_distfiles_window(window);
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
	string percent;
	for (uint y=0; y<bottom_screenline_num and distfile_num<tuidistfiles.size(); y++, distfile_num++){
		if (tuidistfiles[distfile_num].size>0)
			percent="("+toString(tuidistfiles[distfile_num].dld_bytes*100/tuidistfiles[distfile_num].size)+"%) ";
		else
			percent="(n/a%)";

		switch (tuidistfiles[distfile_num].status){
			case DWAITING:
										color_distfile_waiting(window);
										break;
			case DSCRIPTREJECTED:
										color_distfile_script_rejected(window);
										break;
			case DDOWNLOADING:
										color_distfile_downloading(window);
										break;
			case DDOWNLOADED:
										color_distfile_downloaded(window);
										break;
			case DFAILED:
			case DALL_LM_AND_PF_MIRRORS_FAILED:
										color_distfile_failed(window);
										break;
			case DNEW:
			case D_NOT_PROXY_REQUESTED:
			case DPROXY_REJECTED:
			case DPROXY_QUEUED:
			case DPROXY_DOWNLOADING:
			case DPROXY_DOWNLOADED:
			case DPROXY_FAILED:
			default:
										color_distfile_added(window);
										break;
		}
		msg_line(y+1,field("",distfile_num+1,4)+") "
			+percent
			+tuidistfiles[distfile_num].name
			+" ["+tuidistfiles[distfile_num].statusToString()+"]"
			+" Segments: "+toString(tuidistfiles[distfile_num].dld_segments)
			+"/"+toString(tuidistfiles[distfile_num].segments_count)
			+" Bytes: "+toString(tuidistfiles[distfile_num].dld_bytes)
			+"/"+toString(tuidistfiles[distfile_num].size));
	}
//	color_distfiles_window(window);
	wrefresh(window);
//	mainwindow.distfiles_win.add_line(parts[0]+"("+toString(atol(parts[3].c_str())*100/)+"%)"+" "+parts[1]+"/"+parts[2]+" "+parts[3]+"/"+parts[4]);
}



