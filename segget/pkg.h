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
#ifndef __PKG_H__
#define __PKG_H__

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <json/json.h>
#include "distfile.h"

using namespace std;
typedef unsigned int uint;
typedef Tdistfile* Pdistfile;

class Tpkg{
	public:
		vector<Pdistfile> Pdistfile_list;
		string name;
		string category;
		uint distfile_count;
		void load_distfile_list(json_object* json_array_distfile_list);
		Tpkg():
			Pdistfile_list(),
			name(""),
			category(""),
			distfile_count(0)
			{};
		Tpkg(const Tpkg &L); // copy constructor
		Tpkg & operator=(const Tpkg &L);
		~Tpkg();
		void push_back_distfile(json_object* json_distfile);
		void load_pkg_from_json(json_object* json_obj_pkg);
};

extern Tpkg **Ppkg_array;
extern Tpkg proxy_fetcher_pkg;

#endif