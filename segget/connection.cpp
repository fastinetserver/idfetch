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
#include "connection.h"

void Tconnection::start(uint network_number){
	try{
		network_num=network_number;
		total_dld_bytes=0;
		bytes_per_last_interval=0;
		gettimeofday(&start_time,NULL);
		active=true;
		network_array[network_num].connect();
	}catch(...){
		error_log("Error in connection.cpp: start()");
	}
}

void Tconnection::stop(){
	try{
		msg_clean_connection(connection_num);
		active=false;
		network_array[network_num].disconnect();
	}catch(...){
		error_log("Error in connection.cpp: stop()");
	}
}

void Tconnection::inc_bytes_per_last_interval(ulong new_bytes_count){
	try{
		total_dld_bytes+=new_bytes_count;
		bytes_per_last_interval+=new_bytes_count;
	}catch(...){
		error_log("Error in connection.cpp: inc_bytes_per_last_interval()");
	}
}

void Tconnection::show_connection_progress(ulong time_diff){
	try{
		if (active){
			stats.total_bytes_per_last_interval+=bytes_per_last_interval;
			msg_segment_progress(segment->connection_num, network_num,
							segment->segment_num, segment->try_num,
							segment->downloaded_bytes,
							segment->segment_size,
							(bytes_per_last_interval*1000)/time_diff,
							(total_dld_bytes*1000)/time_left_from(start_time));
			bytes_per_last_interval=0;
		}
	}catch(...){
		error_log("Error in connection.cpp: show_connection_progress()");
	}
}