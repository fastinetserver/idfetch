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

#include "mainwindow.h"

void Tmainwindow::connected(){
	for (int line_num=0; line_num<=max_received_screenline_num; line_num++){
		screenlines[line_num]="";
	}
	max_received_screenline_num=0;
	colors_connected();
	set_status("[Connected]");
}

void Tmainwindow::disconnected(){
	colors_disconnected();
	set_status("[Connecting... Attempt:"+toString(attempt_num)+". Waiting for 1 sec, before next reconnect.]");
}
void Tmainwindow::msg_status(){
	msg_short(0,width/2-13,"tuiclient to segget daemon");
	msg_short(0,width-20,"[Lines:"+toString(top_position+1)+"-"+toString(top_position+1+bottom_screenline_num)+"/"+toString(max_received_screenline_num+1)+"]");
	msg_short(height-1,2,status_str);
	msg_short(height-1,width-18,"[h-Help] [q-Quit]");
}

void Tmainwindow::compose(){
	//clear();
	box(window, ACS_VLINE, ACS_HLINE);
	getmaxyx(window,height,width);
	bottom_screenline_num=height-6;
	color_status();
	msg_status();
	color_info();
	for (uint y=bottom_screenline_num,status_line_num=0; y<height-2; y++,status_line_num++){
		msg_line(y+1,screen_info_lines[status_line_num]);
	}
	color_downloads();
	screenlines[26]="Max_num:"+toString(max_received_screenline_num);
	for (int y=0, line_num=top_position; y<bottom_screenline_num; y++, line_num++){
		msg_line(y+1,screenlines[line_num]);
	}
	wrefresh(window);
	//and show children
	help_win.center(height,width);
	help_win.show();
}

void Tmainwindow::set_status(string str){
	status_str=str; 
	notfresh=TRUE;
	//show_lines();
}

void Tmainwindow::set_line(int y, string msg_text){
	screenlines[y]=msg_text;
	notfresh=TRUE;
	if (max_received_screenline_num<y) max_received_screenline_num=y;
}

void Tmainwindow::init(){
	top_position=0;
	max_received_screenline_num=0;
	window=initscr();
//			help_win=Twindow(20,50,5,5);
//			box(help_window, ACS_VLINE, ACS_HLINE);
//			wrefresh(help_window);
	curs_set(0);
	//don't echo keyboard to the screen	visible=TRUE;

	noecho();
	//don't wait for enter
	cbreak();
	//enable arrow keys
	keypad(stdscr,TRUE);
	keypad(window,TRUE);
	exit_flag=FALSE;
	visible=TRUE;
	notfresh=TRUE;
	help_win.init(12,30,5,5);
}
