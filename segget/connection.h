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

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <time.h>
#include <curl/curl.h>
class Tsegment;
#include "segment.h"
#include "utils.h"

using namespace std;

class Tconnection{
	static uint total_connections;
	private:
		uint connection_num;
		uint network_num;
		uint mirror_num;
		ulong total_dld_bytes;
		ulong bytes_per_last_interval;
	public:
		bool active;
		timeval start_time;
		Tsegment *segment;
		Tconnection():
			connection_num(total_connections),
			network_num(0),
			mirror_num(0),
			total_dld_bytes(0),
			bytes_per_last_interval(0),
			active(0),
			start_time(),
			segment(0){total_connections++;};
		void start(CURLM *cm, uint network_number, uint distfile_num, Tsegment *started_segment, uint best_mirror_num);
		void stop(uint connection_result);
		void inc_bytes_per_last_interval(ulong new_bytes_count);
		void show_connection_progress(ulong time_diff);
};

time_t prev_time;
uint Tconnection::total_connections=0;
Tconnection connection_array[MAX_CONNECTS];
#endif