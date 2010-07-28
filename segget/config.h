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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <map>
#include <fstream>
#include "str.h"
#include "tui.h"
using namespace std;

class Tconfig {
	private:
		map<string,string> content_;
		string config_file_name;
	public:
		Tconfig(string const& file_name):
			content_(),
			config_file_name("")
			{config_file_name=file_name;load_settings_from_config_file();};
		void load_settings_from_config_file();
		int set(string const& section, string const& entry, string &dst) const;
		int set_keep_case(string const& section, string const& entry, string &dst) const;
		int set(string const& section, string const& entry, ulong &dst, uint const& min_limit, uint const& max_limit) const;
		int set(string const& section, string const& entry, bool &dst) const;
		void clear();
};

#endif