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

#include "settings.h"

Tsettings settings;

void Tsettings::init(){
	try{
		Tconfig conf("segget.conf");
//		conf.set("logs","general_log_file",general_log_file);
//		conf.set("logs","logs_dir",logs_dir);
//		conf.set("logs","error_log_file",error_log_file);
//		conf.set("logs","debug_log_file",debug_log_file);
		conf.set("request_server","request_ip",request_ip);
		conf.set("request_server","request_port",request_port,1,65535);
		conf.clear();
	}catch(...){
		perror("Error in settings.cpp: init()");
	}
}