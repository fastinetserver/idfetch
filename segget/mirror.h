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
#ifndef __MIRROR_BENCHMARKER_H__
#define __MIRROR_BENCHMARKER_H__

#include <string>
#include <map>
#include "str.h"
#include "tui.h"
#include "settings.h"
using namespace std;

//extern Tsettings settings;

class Tmirror{
	private:
	public:
		uint active_num;
		string url;
		ulong	dld_time;
		ulong	dld_size;
		ulong failed_downloads;
		ulong successful_downloads;
		double	honesty;
		Tmirror():
			active_num(0),
			url(""),
			dld_time(0),
			dld_size(0),
			failed_downloads(0),
			successful_downloads(0),
			honesty(1){};
		void	start();
		void	stop(ulong time, ulong size);
		double mirror_on_the_wall();
		uint get_active_num(){return active_num;};
};

extern map<string,Tmirror *> mirror_list;
string strip_mirror_name(string path);
Tmirror* find_mirror(string mirror_url);
string convert_to_coral_cdn_url(string url_address);
#endif