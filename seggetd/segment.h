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

#ifndef __SEGMENT_H__
#define __SEGMENT_H__
using namespace std;

#include <sstream> 
#include <fstream>
#include <cstring>
#include <string>
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <ncurses.h>
#include <curl/curl.h>
#include "settings.h"

#include "distfile.h"
class Tdistfile;
#include "stats.h"


#define MAX_CONNECTS 20
//extern Tsettings settings;
//unsigned long downloaded_bytes=0;
size_t write_data(void *buffer, size_t size, size_t nmemb, void *cur_segment);

enum Tsegment_status{SWAITING, SDOWNLOADING, SDOWNLOADED, SFAILED};

class Tsegment{
	private:
		CURL *easyhandle;
		char* urllist;
	public:
		string file_name;
		Tsegment_status status;
		uint try_num;
		Tdistfile* parent_distfile;
		uint connection_num;
		uint segment_num;
		uint segment_size;
		unsigned long downloaded_bytes;
		string url;
		string range;
		ofstream segment_file;
		Tsegment():
			easyhandle(0),
			urllist(0),
			file_name(""),
			status(SWAITING),
			try_num(0),
			parent_distfile(0),
			connection_num(0),
			segment_num(0),
			segment_size(1000),
			downloaded_bytes(0),
			url(""),
			range(""),
			segment_file(0)
			{};
		Tsegment(const Tsegment &L);             // copy constructor
		Tsegment & operator=(const Tsegment &L);
		~Tsegment();
		bool segment_verification_is_ok();
		void set_segment(Tdistfile *prnt_distfile, uint seg_num, string distfile_name, ulong default_seg_size, ulong range_end);
		string get_file_name(){return file_name;};
		uint start(CURLM *cm, uint con_num, uint network_num, uint distfile_num, string url);
		uint add_easy_handle_to_multi(CURLM *cm, uint network_num);
};

extern Tsegment *segments_in_progress[MAX_CONNECTS];

string statusToString(Tsegment_status the_status);
void show_progress(double time_diff);
size_t write_data(void *buffer, size_t size, size_t nmemb, void *cur_segment);
#endif