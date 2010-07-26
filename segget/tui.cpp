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
vector<string> log_lines;
uint log_lines_counter=0;
uint max_published_screenline_num;

//bool msg_idle=true;
void msg(uint y, uint x, string msg_text){
//	if (msg_idle){
//		msg_idle=false;
		try{
			if (max_published_screenline_num<y && y<MAX_LINES) max_published_screenline_num=y;
			screenlines[y]=msg_text;
			ui_server.send_connection_msg_to_all_clients(y,msg_text);
//			string ready_msg_text=msg_text+"                        ";
			string ready_msg_text=msg_text+"";
			mvaddstr(y,x,ready_msg_text.c_str());
			refresh();
		}catch(...){
			error_log_no_msg("Error in tui.cpp: msg()");
		}
//		msg_idle=true;
//	}
}

void msg_segment_progress(uint connection_num, uint network_num, uint segment_num, uint try_num, ulong dld_bytes, ulong total_bytes, ulong speed, ulong avg_speed){
	try{

		string eta_string;
		if (speed==0){
			eta_string=" ETA: inf";
		}else{
			eta_string=" ETA: "+secsToString((total_bytes-dld_bytes)/speed);
		}

		string speed_str;
		string avg_speed_str;
/*
		if (speed<1000)
			speed_str=field(" Speed:",speed,7)+" b/s";
		else
			speed_str=field(" Speed:",speed/1000,7)+" Kb/s";
*/

		speed_str=" Speed: "+speedToString(speed);
/*
		if (avg_speed<1000)
			avg_speed_str=field(" AVG",avg_speed,7)+" b/s";
		else
			avg_speed_str=field(" AVG",avg_speed/1000,7)+" Kb/s";
*/
		avg_speed_str=" AVG speed: "+speedToString(avg_speed);
		
		int percent=dld_bytes*100/total_bytes;

		
		msg(connection_num*CONNECTION_LINES,0,
			field("[",connection_num,2)+"]"
			+field(" Net",network_num,1)
			+field(" Segment:",segment_num, 5)
			+field(" Try:",try_num,4)
			+field(" Bytes:",dld_bytes,7)
			+field(" / ",total_bytes,7)
			+field(" = ",percent,3)+"%"
			+speed_str
			+avg_speed_str
			+eta_string);
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
/*
void msg_status1(uint connection_num, uint segment_num, string msg_text){
	try{
		msg(connection_num*CONNECTION_LINES+2,0,"Seg#"+toString(segment_num)+" "+msg_text);
	}catch(...){
		error_log_no_msg("Error in tui.cpp: msg_status1()");
	}
}
void msg_status2(uint connection_num, string msg_text){
	try{
		msg(connection_num*CONNECTION_LINES+3,0,msg_text);
	}catch(...){
		error_log_no_msg("Error in tui.cpp: msg_status2()");
	}
}
*/

void msg_clean_connection(uint connection_num){
	try{
		msg(connection_num*CONNECTION_LINES,0,"");
		msg(connection_num*CONNECTION_LINES+1,0,"");
		msg(connection_num*CONNECTION_LINES+2,0,"");
		msg(connection_num*CONNECTION_LINES+3,0,"");

//		msg(connection_num*CONNECTION_LINES,0,"                                                                                                                  ");
//		msg(connection_num*CONNECTION_LINES+1,0,"                                                                                                                  ");
//		msg(connection_num*CONNECTION_LINES+2,0,"                                                                                                                  ");
//		msg(connection_num*CONNECTION_LINES+3,0,"                                                                                                                  ");
	}catch(...){
		error_log_no_msg("Error in tui.cpp: msg_clean_connection()");
	}
}
void msg_log(string log_text){
	try{
		ui_server.send_log_msg_to_all_clients(log_text);
		log_lines.push_back(log_text);
		if (log_lines.size()>LOG_LINES_MAX_NUM){
			log_lines.erase(log_lines.begin(),log_lines.begin()+log_lines.size()-LOG_LINES_MAX_NUM);
		}
//		msg(20,0, error_text);
	}catch(...){
		error_log_no_msg("Error in tui.cpp: msg_error()");
	}
}
void msg_error_log(string error_log_text){
	try{
		ui_server.send_error_log_msg_to_all_clients(error_log_text);
		log_lines.push_back(error_log_text);
		if (log_lines.size()>LOG_LINES_MAX_NUM){
			log_lines.erase(log_lines.begin(),log_lines.begin()+log_lines.size()-LOG_LINES_MAX_NUM);
		}
//		msg(20,0, error_text);
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