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
#include <ncurses.h>

using namespace std;

int atoi(string num_str){
	return atoi(num_str.c_str());
}

long atol(string num_str){
	return atol(num_str.c_str());
}

Tparts split(string splitter, string str){
	Tparts result;
	int splitter_pos=str.find(splitter);
	result.before=str.substr(0,splitter_pos);
	result.after=str.substr(splitter_pos+splitter.length());
	return result;
}

vector <string> split_to_vector(string splitter, string str){
	vector <string> parts;
	uint splitter_pos=str.find(splitter);
	while (splitter_pos!=str.npos){
		parts.push_back(str.substr(0,splitter_pos));
		str=str.substr(splitter_pos+splitter.length());
		splitter_pos=str.find(splitter);
	}
	if (str.length()>0){
		parts.push_back(str);
	}
	return parts;
}
/*
template<typename T> string toString(T t){
	stringstream s;
	s << t;
	return s.str();
}
*/

string colorToString(uint a_color){
	switch (a_color){
		case COLOR_BLACK: return "COLOR_BLACK";
		case COLOR_RED: return "COLOR_RED";
		case COLOR_GREEN: return "COLOR_GREEN";
		case COLOR_YELLOW: return "COLOR_YELLOW";
		case COLOR_BLUE: return "COLOR_BLUE";
		case COLOR_MAGENTA: return "COLOR_MAGENTA";
		case COLOR_CYAN: return "COLOR_CYAN";
		case COLOR_WHITE: return "COLOR_WHITE";
		default: return "UNKNOWN COLOR";
	}
}

string toString(uint t){
	stringstream s;
	s << t;
	return s.str();
}
string toString(int t){
	stringstream s;
	s << t;
	return s.str();
}
string toString(ulong t){
	stringstream s;
	s << t;
	return s.str();
}
string toString(long t){
	stringstream s;
	s << t;
	return s.str();
}
string toString(bool t){
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

string field(string prefix,ulong t, int width){
	try{
		stringstream s1,s2;
		s1 << t;
		width=width+prefix.length();
		s2.width(width);
		s2 << prefix+s1.str();
		return s2.str();
	}catch(...){
		error_log("Error in str.cpp: field()");
		return "";
	}
}
