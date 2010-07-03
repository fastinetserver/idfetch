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

#ifndef __DISTFILE_H__
#define __DISTFILE_H__
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#  include <unistd.h>
#endif
#include <curl/multi.h>

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <json/json.h>
#include "segment.cpp"
#include "mirror.cpp"
#include "checksum.cpp"
#include "network.h"
#include "networkbroker.h"

using namespace std;

typedef unsigned int uint;

class Tdistfile{
	private:
		uint dld_segments_count;
		Tnetworkbroker networkbrokers_array[MAX_NETWORKS];
		bool choose_best_mirror(CURLM* cm, uint connection_num, uint network_num, uint seg_num);
	public:
		bool downloaded;
		uint active_connections_num;
		string *url_list;
		uint url_num;
		uint segment_num;
		uint segments_count;
		Tsegment *dn_segments;
		string name;
		uint num;
		ulong size;
		string RMD160;
		string SHA1;
		string SHA256;
		uint url_count;
		uint segment_size;
		Tdistfile():
			dld_segments_count(0),
			downloaded(0),
			active_connections_num(0),
			url_list(0),
			url_num(0),
			segment_num(0),
			segments_count(0),
			dn_segments(0),
			name(""),
			num(0),
			size(0),
			RMD160(""),
			SHA1(""),
			SHA256(""),
			url_count(0),
			segment_size(settings.max_segment_size)
			{init();};
		Tdistfile(const Tdistfile &L);             // copy constructor
		Tdistfile & operator=(const Tdistfile &L);
		~Tdistfile();
		void init();
		void load_distfile_from_json(json_object* json_obj_distfile);
		void load_url_list(json_object* json_array_distfile_urllist);
		void split_into_segments();
		int provide_segment(CURLM* cm, uint connection_num, uint seg_num);
		void inc_dld_segments_count(Tsegment * current_segment);
		int combine_segments();
		bool check_if_dld();
};

#endif