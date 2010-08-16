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

//bool msg_idle=true;
void log(string log_msg_text){
	try{
		log_no_msg(log_msg_text);
	}catch(...){
		error_log("Error in tui.cpp: log()");
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
				file << log_msg_text << endl;
				file.close();
			}
			catch(...){
				error_log("Error while writing "+settings.logs_dir+"/"+settings.general_log_file+".");
				return;
			}
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
		if (settings.debug_log_file!="none"){
			ofstream file;
			file.exceptions (ofstream::failbit | ofstream::badbit);
			try{
				file.open((settings.logs_dir+"/"+settings.debug_log_file).c_str(), ios::app);
				if (! file.is_open()){
					error_log("Error opening "+settings.logs_dir+"/"+settings.debug_log_file+" for writing.");
					error_log("....Check if folder "+settings.logs_dir+" exists and seggetd has permissions to write into it.");
					return;
				}
			}
			catch(...){
				error_log("Error opening "+settings.logs_dir+"/"+settings.debug_log_file+" for writing.");
				error_log("....Check if folder "+settings.logs_dir+" exists and seggetd has permissions to write into it.");
				return;
			}
			try{
				file << debug_msg_text << endl;
				file.close();
			}catch(...){
				error_log("Error while writing "+settings.logs_dir+"/"+settings.debug_log_file+".");
				return;
			}
		}
	}catch(...){
		error_log("Error in tui.cpp: debug()");
	}
}

void error_log_no_msg(string error_msg_text){
	try{
		if (settings.error_log_file!="none"){
			ofstream file ((settings.logs_dir+"/"+settings.error_log_file).c_str(), ios::app);
			file << error_msg_text << endl;
			file.close();
		}
	}catch(...){
//		fprintf(stderr, "Error opening error log file.");
//		fprintf(stderr, "Error log file: %s/%s",settings.logs_dir.c_str(),settings.error_log_file.c_str());
	}
}

void error_log(string error_msg_text){
	try{
		error_log_no_msg(error_msg_text);
	}catch(...){
//		fprintf(stderr, "Error opening error log file.");
//		fprintf(stderr, "Error log file: %s/%s",settings.logs_dir.c_str(),settings.error_log_file.c_str());
	}
}