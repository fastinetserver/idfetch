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

const uint CONNECTION_LINES=5;

//bool msg_idle=true;
void msg(uint y, uint x, string msg_text){
//	if (msg_idle){
//		msg_idle=false;
		try{
			if (max_published_screenline_num<y && y<MAX_LINES) max_published_screenline_num=y;
			screenlines[y]=msg_text;
			ui_server.send_all_clients(y,msg_text);
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

void msg_connecting(uint connection_num, uint distfile_num, uint segment_num, string msg_text){
	try{
		msg(connection_num*CONNECTION_LINES+1,0,"DF#"+toString(distfile_num)+" Seg#"+toString(segment_num)+" "+msg_text);
	}catch(...){
		error_log_no_msg("Error in tui.cpp: msg_connecting()");
	}
}

void msg_segment_progress(uint connection_num, uint network_num, uint segment_num, uint try_num, ulong dld_bytes, ulong total_bytes, ulong speed, ulong avg_speed){
	try{
		string speed_str;
		string avg_speed_str;
		if (speed<1000)
			speed_str=field(" Speed:",speed,7)+" b/s";
		else
			speed_str=field(" Speed:",speed/1000,7)+" Kb/s";
		if (avg_speed<1000)
			avg_speed_str=field(" AVG speed:",avg_speed,7)+" b/s";
		else
			avg_speed_str=field(" AVG speed:",avg_speed/1000,7)+" Kb/s";
		int percent=dld_bytes*100/total_bytes;
		msg(connection_num*CONNECTION_LINES,0,
			field("[",connection_num,2)+"]"
			+field(" Net",network_num,1)
			+field(" Segment:",segment_num, 5)
			+field(" Try:",try_num,4)
			+field(" Bytes:",dld_bytes,7)
			+field(" / ",total_bytes,7)
			+field(" = ",percent,3)+"%%"
			+speed_str
			+avg_speed_str);
	}catch(...){
		error_log_no_msg("Error in tui.cpp: msg_segment_progress()");
	}
}

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
void msg_error(string error_text){
	try{
		msg(20,0, error_text);
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
void log(string log_msg_text){
	try{
		msg(LOG_LINE_NUM,0, "LOG:"+log_msg_text);
		ofstream file;
		file.exceptions (ofstream::failbit | ofstream::badbit);
		try{
			file.open((settings.logs_dir+"/"+settings.general_log_file).c_str(), ios::app);
		}catch(...){
			error_log("Error opening "+settings.logs_dir+"/"+settings.general_log_file+".");
			return;
		}
		try{
			file << log_msg_text << endl;
			file.close();
		}
		catch(...){
			error_log("Error while writing "+settings.logs_dir+"/"+settings.general_log_file+".");
			return;
		}
	}catch(...){
		error_log("Error in tui.cpp: log()");
	}
}
void debug(string debug_msg_text){
	try{
//		msg(DEBUG_LINE_NUM,0, "DEBUG:"+debug_msg_text);
		debug_no_msg(debug_msg_text);
	}catch(...){
		error_log("Error in tui.cpp: debug()");
	}
}

void debug_no_msg(string debug_msg_text){
	try{
		ofstream file;
		file.exceptions (ofstream::failbit | ofstream::badbit);
		try{
			file.open((settings.logs_dir+"/"+settings.debug_log_file).c_str(), ios::app);
		}
		catch(...){
			error_log("Error opening "+settings.logs_dir+"/"+settings.debug_log_file+".");
			return;
		}
		try{
			file << debug_msg_text << endl;
			file.close();
		}
		catch(...){
			error_log("Error while writing "+settings.logs_dir+"/"+settings.debug_log_file+".");
			return;
		}
	}catch(...){
		error_log("Error in tui.cpp: debug()");
	}
}

void error_log_no_msg(string error_msg_text){
	try{
		ofstream file ((settings.logs_dir+"/"+settings.error_log_file).c_str(), ios::app);
		file << error_msg_text << endl;
		file.close();
	}catch(...){
		fprintf(stderr, "Error opening error log file.");
		fprintf(stderr, "Error log file: %s/%s",settings.logs_dir.c_str(),settings.error_log_file.c_str());
	}
}

void error_log(string error_msg_text){
	try{
		ofstream file ((settings.logs_dir+"/"+settings.error_log_file).c_str(), ios::app);
		file << error_msg_text << endl;
		file.close();
	}catch(...){
		fprintf(stderr, "Error opening error log file.");
		fprintf(stderr, "Error log file: %s/%s",settings.logs_dir.c_str(),settings.error_log_file.c_str());
	}
	try{
		msg(ERROR_LINE_NUM,0, "ERROR:"+error_msg_text);
	}catch(...){
		error_log_no_msg("Error in tui.cpp: error_log()");
	}
}