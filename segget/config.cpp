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
#include "str.h"
#include "tui.h"
using namespace std;

map<string,string> content_;

class ConfigFile {
	private:
		string config_file_name;
	public:
		ConfigFile(string const& file_name):
			config_file_name("")
			{config_file_name=file_name;load_settings_from_config_file();};
		void load_settings_from_config_file();
		int set(string &dst,string const& section, string const& entry) const;
		int set(ulong &dst,string const& section, string const& entry, uint const& min_limit, uint const& max_limit) const;
		int set(bool &dst,string const& section, string const& entry) const;
};

#endif
#include <fstream>

string trim(std::string const& source, char const* delims = " \t\r\n") {
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
}

void ConfigFile::load_settings_from_config_file() {

	ifstream file;
	file.exceptions (ifstream::failbit | ifstream::badbit);
	try{
		file.open(config_file_name.c_str());
	}
	catch(...){
		error_log("Error opening settings file: "+config_file_name+". Default settings will be used. Check if config file: "+config_file_name+" exists and segget has rights to access it.");
		return;
	}
	try{
		//processing file
		string line;
		string name;
		string value;
		string inSection;
		int posEqual;
		while (not(file.eof())) {
			getline(file,line);
			if (! line.length()) continue;
			if (line[0] == '#') continue;
			if (line[0] == ';') continue;

			line=noupper(line);
			if (line[0] == '[') {
				inSection=trim(line.substr(1,line.find(']')-1));
				continue;
			}

			posEqual=line.find('=');
			name  = trim(line.substr(0,posEqual));
			value = trim(line.substr(posEqual+1));

			content_[inSection+'/'+name]=noupper(value);
		}
	}
	catch(...){
		error_log("Settings file: "+config_file_name+" was opened, but an error occured while reading settings from it.");
	}
}

int ConfigFile::set(string &dst, string const& section, string const& entry) const {
	map<string,string>::const_iterator ci = content_.find(section + '/' + entry);

	if (ci == content_.end()){
		log("! Settings in file:"+config_file_name+" ["+section+"]."+entry+" has not been set.");
		log("! Settings in file:"+config_file_name+" ["+section+"]."+entry+"="+dst+". Default value forced.");
		return 1;
	}
	else{
		dst=ci->second;
		log("Settings in file:"+config_file_name+" ["+section+"]."+entry+"="+dst);
		return 0;
	}
}
int ConfigFile::set(ulong &dst, string const& section, string const& entry, uint const& min_limit, uint const& max_limit) const {
	uint return_value;
	map<string,string>::const_iterator ci = content_.find(section + '/' + entry);

	if (ci == content_.end()){
		log("! Settings in file:"+config_file_name+" ["+section+"]."+entry+" has not been set.");
		log("! Settings in file:"+config_file_name+" ["+section+"]."+entry+"="+toString(dst)+". Default value forced.");
		return 1;
	}
	else{
		return_value=atoi(ci->second.c_str());
		if (return_value==0)
			if (toString(return_value)!=ci->second){
				log("! Settings in file:"+config_file_name+" ["+section+"]."+entry
						+" must have an integer value in range from "+toString(min_limit)
						+" to "+toString(max_limit)
						+". Can't convert "+ci->second
						+" to integer. ");
				log("! Settings in file:"+config_file_name+" ["+section+"]."+entry+"="+toString(dst)+". Default value forced.");
				return 1;
			}
		if ((return_value>=min_limit) and (return_value<=max_limit)){
			//everything is ok
			log("Settings in file:"+config_file_name+" ["+section+"]."+entry+"="+toString(return_value));
			dst=return_value;
			return 0;
		}else{
			log("! Settings in file:"+config_file_name+" ["+section+"]."+entry
				+" must have an integer value in range from "+toString(min_limit)
				+" to "+toString(max_limit)+".");
				log("! Settings in file:"+config_file_name+" ["+section+"]."+entry+"="+toString(dst)+". Default value forced.");
			return 1;
		}
	}
}
int ConfigFile::set(bool &dst, string const& section, string const& entry) const {
	uint return_value;
	map<std::string,string>::const_iterator ci = content_.find(section + '/' + entry);

	if (ci == content_.end()){
		log("! Settings in file:"+config_file_name+" ["+section+"]."+entry+" has not been set.");
		log("! Settings in file:"+config_file_name+" ["+section+"]."+entry+"="+toString(dst)+". Default value forced.");
		return 1;
	}
	else{
		return_value=atoi(ci->second.c_str());
		if (return_value==0)
			if (toString(return_value)!=ci->second){
				log("! Settings in file:"+config_file_name+" ["+section+"]."+entry
						+" must have a boolean value: 0 or 1"
						+". Can't convert "+ci->second
						+" to 0 or 1. ");
				log("! Settings in file:"+config_file_name+" ["+section+"]."+entry+"="+toString(dst)+". Default value forced.");
				return 1;
			}
		if ((return_value==0) or (return_value==1)){
			//everything is ok
			log("Settings in file:"+config_file_name+" ["+section+"]."+entry+"="+toString(return_value));
			dst=return_value;
			return 0;
		}else{
			log("! Settings in file:"+config_file_name+" ["+section+"]."+entry
				+" must have a boolean value: 0 or 1");
			log("! Settings in file:"+config_file_name+" ["+section+"]."+entry+"="+toString(dst)+". Default value forced.");
			return 1;
		}
	}
}