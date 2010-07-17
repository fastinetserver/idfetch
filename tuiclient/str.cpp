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

#include "str.h"
using namespace std;

template<typename T> string toString(T t){
	stringstream s;
	s << t;
	return s.str();
}

template<typename T> string field(string prefix,T t, int width){
	try{
		stringstream s1,s2;
		s1 << t;
		width=width+prefix.length();
		s2.width(width);
		s2 << prefix+s1.str();
		return s2.str();
	}catch(...){
		return "";
	}
}

int lower_char(int c){
	try{
		return tolower((unsigned char)c);
	}catch(...){
		error_log("Error in str.cpp: lower_char()");
		return 0;
	}
}

string noupper(string s){
	try{
		transform(s.begin(), s.end(), s.begin(), lower_char);
		return s;
	}catch(...){
		error_log("Error in str.cpp: noupper()");
		return "";
	}
}

string trim(std::string const& source, char const* delims) {
	try{
		string result(source);
		string::size_type index = result.find_last_not_of(delims);
		if(index != string::npos)
			result.erase(++index);
	
		index = result.find_first_not_of(delims);
		if(index != string::npos)
			result.erase(0, index);
		else
			result.erase();
		return result;
	}catch(...){
		error_log("Error in str.cpp: trim()");
		return "";
	}
}