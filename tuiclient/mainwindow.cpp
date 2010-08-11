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
#include "log.h"
#include "str.h"

string get_time(string time_format){
	try{
		time_format=time_format+" ";
		time_t rawtime;
		struct tm * timeinfo;
		char buffer [80];
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		strftime(buffer,80,time_format.c_str(),timeinfo);
		return buffer;
	}catch(...){
		error_log("Error in log.cpp: get_time()");
		return "";
	}
}

void Tmainwindow::connected(){
	if (disconnected_flag){
		scroll_lines.clear();
		max_received_screenline_num=0;
		tuidistfiles.clear();
		colors_connected();
		set_status("[Connected]");
		disconnected_flag=false;
//		set_line(2," CONNECTED "+get_time("%X"));
	}
//	for (int line_num=0; line_num<=max_received_screenline_num; line_num++){
//		screenlines[line_num]="";
//	}

//	notfresh=true;
}

void Tmainwindow::disconnected(){
	colors_disconnected();
	set_status("[Connecting... Attempt:"+toString(attempt_num)+". Waiting for 1 sec, before next reconnect.]");
	disconnected_flag=true;
//	notfresh=true;
}
void Tmainwindow::msg_status(){
	msg_short(0,2,"Tuiclient "+status_str);
	msg_short(0,width-40,"[h-Help] [q-Quit]");
	msg_short(0,width-20,"[Lines:"+toString(top_position+1)+"-"+toString(top_position+1+bottom_screenline_num)+"/"+toString(max_received_screenline_num+1)+"]");
//	msg_short(height-1,2,status_str);
//	msg_short(height-1,width-18,"");
}

void Tmainwindow::compose(){
//	wclear(window);
//	box(window, ACS_VLINE, ACS_HLINE);
	getmaxyx(window,height,width);
//	resize(height,width,0,0);
//	screenlines[27]=sms;
	bottom_screenline_num=height-2;
	color_status(window);
	make_frame();
	msg_status();
	color_info(window);
	msg_short(height-1,2,screen_info_lines[0]);
	color_downloads(window);
//	screenlines[26]="Max_num:"+toString(max_received_screenline_num);
//	for (int y=0, line_num=top_position; y<bottom_screenline_num; y++, line_num++){
//		msg_line(y+1,screenlines[line_num]);
//	}
	for (uint y=0, line_num=top_position; y<bottom_screenline_num; y++, line_num++){
		if (line_num<scroll_lines.size()){
			msg_line(y+1,scroll_lines[line_num]);
		}else{
			msg_line(y+1,"");
		}
	}
	wrefresh(window);
	//and show children
	if (log_win.visible && error_log_win.visible){
		int modd = (height-2)%4;
		log_win.resize((height-2)/4+modd, width, 1+(height-2)/4*2, 0);
		error_log_win.resize((height-2)/4, width, 1+(height-2)/4*3+modd, 0);
	}else{
		int modd = (height-2)%2;
		log_win.resize((height-2)/2+modd, width, 1+(height-2)/2, 0);
		error_log_win.resize((height-2)/2+modd, width, 1+(height-2)/2, 0);
	}
	int modd = (height-2)%2;
	distfiles_win.resize((height-3)/2+modd, width, 1+(height-3)/2, 0);

	distfiles_win.show();

	log_win.show();
	error_log_win.show();

	help_win.center(height,width);
	help_win.show();
}

void Tmainwindow::set_status(string str){
	status_str=str; 
	notfresh=TRUE;
	//show_lines();
}

void Tmainwindow::set_line(uint y, string msg_text){
	while (scroll_lines.size()<=y){
		scroll_lines.push_back("");
	};
	scroll_lines[y]=msg_text;
	notfresh=TRUE;
	if (max_received_screenline_num<y) max_received_screenline_num=y;
}

void Tmainwindow::init(){
	top_position=0;
	max_received_screenline_num=0;
	window=initscr();
//	clear();
//			help_win=Twindow(20,50,5,5);
//			box(help_window, ACS_VLINE, ACS_HLINE);
//			wrefresh(help_window);
	curs_set(0);
	//don't echo keyboard to the screen	visible=TRUE;

	noecho();
//	nodelay(window,1);
	//don't wait for enter
	cbreak();
	//enable arrow keys
	keypad(stdscr,true);
	keypad(window,true);
//	exit_flag=FALSE;
	visible=true;
	notfresh=true;
//	wclear(window);
//	wrefresh(window);
//	refresh();
	help_win.init(" HELP ",14,31,5,5);
	log_win.init(" LOG ",12,50,5,5);
	distfiles_win.init(" DISTFILES ",12,50,5,5);
	distfiles_win.visible=true;
	error_log_win.init(" ERROR LOG ",12,50,5,5);
	disconnected();
	wclear(window);
//	sleep(1);
	show();
	sleep(1);
}
