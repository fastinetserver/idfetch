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

#include "log.h"

vector<string> log_lines;
vector<string> error_log_lines;

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

void log_no_msg(string log_msg_text){
	try{
		if (settings.general_log_file!="none"){
			ofstream file;
			file.exceptions (ofstream::failbit | ofstream::badbit);
			try{
				file.open((settings.logs_dir+"/"+settings.general_log_file).c_str(), ios::app);
			}catch(...){
				error_log("Error opening "+settings.logs_dir+"/"+settings.general_log_file+".");
				return;
			}
			try{
				file << get_time(settings.general_log_time_format) << log_msg_text << endl;
				file.close();
			}
			catch(...){
				error_log("Error while writing "+settings.logs_dir+"/"+settings.general_log_file+".");
				return;
			}
		}
	}catch(...){
		error_log("Error in log.cpp: log()");
	}
}

void log(string log_msg_text){
	log_no_msg(log_msg_text);
	try{
		log_lines.push_back(log_msg_text);
		if (log_lines.size()>LOG_LINES_MAX_NUM){
			log_lines.erase(log_lines.begin(),log_lines.begin()+log_lines.size()-LOG_LINES_MAX_NUM);
		}
		msg_log(get_time(settings.general_log_time_format)+log_msg_text);
	}catch(...){
		error_log("Error in log.cpp: log()");
	}
}
			
void debug_no_msg(string debug_msg_text){
	try{
		if (settings.debug_log_file!="none"){
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
				file << get_time(settings.debug_log_time_format) << debug_msg_text << endl;
				file.close();
			}
			catch(...){
				error_log("Error while writing "+settings.logs_dir+"/"+settings.debug_log_file+".");
				return;
			}
		}
	}catch(...){
		error_log("Error in log.cpp: debug()");
	}
}

void debug(string debug_msg_text){
	debug_no_msg(debug_msg_text);
	try{
//		msg(DEBUG_LINE_NUM,0, "DEBUG:"+get_time(settings.debug_time_format)+debug_msg_text);
	}catch(...){
		error_log("Error in log.cpp: debug()");
	}
}

void error_log_no_msg(string error_msg_text){
	try{
		if (settings.error_log_file!="none"){
			ofstream file ((settings.logs_dir+"/"+settings.error_log_file).c_str(), ios::app);
			file << get_time(settings.error_log_time_format) << error_msg_text << endl;
			file.close();
		}
	}catch(...){
		fprintf(stderr, "Error opening error log file.");
		fprintf(stderr, "Error log file: %s/%s",settings.logs_dir.c_str(),settings.error_log_file.c_str());
	}
}

void error_log(string error_msg_text){
	error_log_no_msg(error_msg_text);
	try{
		error_log_lines.push_back(error_msg_text);
		if (error_log_lines.size()>LOG_LINES_MAX_NUM){
			error_log_lines.erase(error_log_lines.begin(),error_log_lines.begin()+error_log_lines.size()-LOG_LINES_MAX_NUM);
		}
		msg_error_log(get_time(settings.error_log_time_format)+error_msg_text);
	}catch(...){
		error_log_no_msg("Error in log.cpp: error_log()");
	}
}