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

#include "tui.h"
extern Tsettings settings;

const uint CONNECTION_LINES=3;
string screenlines[DEBUG_LINE_NUM+1];
uint log_lines_counter=0;
uint max_published_screenline_num;

void msg(uint y, uint x, string msg_text){
		try{
			if (max_published_screenline_num<y && y<MAX_LINES) max_published_screenline_num=y;
			screenlines[y]=msg_text;
			ui_server.send_connection_msg_to_all_clients(y,msg_text);
			string ready_msg_text=msg_text+"";
			mvaddstr(y,x,ready_msg_text.c_str());
			refresh();
		}catch(...){
			error_log_no_msg("Error in tui.cpp: msg()");
		}
}

void msg_segment_progress(uint connection_num, string progress_text){
	try{
		msg(connection_num*CONNECTION_LINES,0,
			field("[",connection_num,2)+"]"+progress_text);
	}catch(...){
		error_log_no_msg("Error in tui.cpp: msg_segment_progress()");
	}
}
void msg_connecting(uint connection_num, uint distfile_num, uint segment_num, string msg_text){
	try{
		msg(connection_num*CONNECTION_LINES+1,0,"DF#"+toString(distfile_num)+" Seg#"+toString(segment_num)+" "+msg_text);
	}catch(...){
		error_log_no_msg("Error in tui.cpp: msg_connecting()");
	}
}

void msg_clean_connection(uint connection_num){
	try{
		msg(connection_num*CONNECTION_LINES,0,"");
		msg(connection_num*CONNECTION_LINES+1,0,"");
		msg(connection_num*CONNECTION_LINES+2,0,"");
		msg(connection_num*CONNECTION_LINES+3,0,"");
	}catch(...){
		error_log_no_msg("Error in tui.cpp: msg_clean_connection()");
	}
}

void msg_log(string log_text){
	try{
		ui_server.send_log_msg_to_all_clients(log_text);
	}catch(...){
		error_log_no_msg("Error in tui.cpp: msg_error()");
	}
}
void msg_error_log(string error_log_text){
	try{
		ui_server.send_error_log_msg_to_all_clients(error_log_text);
	}catch(...){
		error_log_no_msg("Error in tui.cpp: msg_error()");
	}
}

void msg_total(string msg_text){
	try{
		msg(TOTAL_LINE_NUM,0,msg_text);
	}catch(...){
		error_log_no_msg("Error in tui.cpp: msg_total()");
	}
}