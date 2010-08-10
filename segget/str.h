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

#ifndef __STR_H__
#define __STR_H__
#include <string>
#include <algorithm>
#include <sstream>
#include "tui.h"
using namespace std;

//template<typename T> string toString(T t);
//template<typename T> string field(string prefix,T t, int width);

string trim(std::string const& source, char const* delims = " \t\r\n");
int lower_char(int c);
string noupper(string s);
bool split(string splitter, string str, string &before, string &after);
vector <string> split_to_vector(string splitter, string str);
string toString(uint t);
string toString(int t);
string toString(ulong t);
string toString(long t);
string toString(bool t);
string toString(double t);
string field(string prefix,ulong t, int width);

#endif