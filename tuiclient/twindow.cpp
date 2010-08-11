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

#include "twindow.h"
#include "str.h"
string sms;

void Twindow::init(string caption_, int height_, int width_, int y_, int x_){
	caption=caption_;
	height=height_;
	width=width_;
	y=y_;
	x=x_;
	window=newwin(height,width,y,x);
}

void Twindow::toggle(){
	visible=not(visible);
}

void Twindow::msg_line(uint y, string msg_text){
	try{
//		cout << msg_text;
		msg_text=msg_text+"                                                                                                                                                         ";
		mvwaddstr(window,y,1,msg_text.substr(0,width-2).c_str());
//		mvwprintw(screen,y,1,ready_msg_text.c_str());
//		mvwprintw(screen,y,1,msg_text.substr(0,max_x-2).c_str());
//		mvaddstr();
	}catch(...){
//		error_log_no_msg("Error in tui.cpp: msg()");
	}
}

void Twindow::msg_short(uint y, uint x, string msg_text){
	try{
		mvwaddstr(window,y,x,msg_text.c_str());
	}catch(...){
//		error_log_no_msg("Error in tui.cpp: msg()");
	}
}
void Twindow::up(int inc){
	int tmp_top_position=top_position-inc;
	notfresh=TRUE;
	if (tmp_top_position<0){
		top_position=0;
	}else{
		top_position=tmp_top_position; 
	}
//	sms="UP"+toString(top_position);
//	refresh();
}
void Twindow::down(uint inc){
	uint tmp_top_position=top_position+inc;
	// if we need to scroll down
	if (max_received_screenline_num>bottom_screenline_num){
		notfresh=TRUE;
		if (tmp_top_position>(max_received_screenline_num-bottom_screenline_num)){
		//set to max if it's over the limit
			top_position=max_received_screenline_num-bottom_screenline_num; 
		}else{
			top_position=tmp_top_position; 
		}
	}
//	sms="DOWN "+toString(top_position)+"------>"+toString(bottom_screenline_num);
//	refresh();
}

void Twindow::center(uint max_y, uint max_x){
	y=(max_y-height)/2;
	x=(max_x-width)/2;
	mvwin(window,y,x);
}

void Twindow::resize(uint new_height, uint new_width, uint new_y, uint new_x){
	y=new_y;
	x=new_x;
	height=new_height;
	width=new_width;
	mvwin(window,y,x);
	wresize(window,height,width);
}

void Twindow::make_frame(){
//	wclear(window);
	box(window, ACS_VLINE, ACS_HLINE);
	mvwaddstr(window,0,(width-caption.length())/2,caption.c_str());
//	msg_short(0,width-20,"[Lines:"+toString(top_position+1)+"-"+toString(top_position+bottom_screenline_num)+"/"+toString(max_received_screenline_num)+"]");
//	msg_short(0,width-20,"[Lines:"+toString(top_position+1)+"-"+toString(top_position+bottom_screenline_num)+"/"+toString(screen_lines.size())+"]");
}

void Twindow::refresh(){
	if (notfresh){
		show();
	}
}

void Twindow::show(){
	if (visible){
		compose();
	}
}