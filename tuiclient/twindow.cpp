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

void Twindow::init(int height_, int width_, int y_, int x_){
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
}
void Twindow::down(uint inc){
	int tmp_top_position=top_position+inc;
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
}

void Twindow::center(uint max_y, uint max_x){
	y=(max_y-height)/2;
	x=(max_x-width)/2;
	mvwin(window,y,x);
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