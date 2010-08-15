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

#include "mirror.h"

map<string,Tmirror *> mirror_list;

string strip_mirror_name(string path){
	try{
		string mirror_name;
		mirror_name=path.substr(0,path.find("/",(path.find("://",0)+3)));
		return mirror_name;
	}catch(...){
		error_log("Error in mirror.cpp: strip_mirror_name()");
		return "";
	}
}

string convert_to_coral_cdn_url(string url_address){
	try{
		string protocol, after_protocol;
		if (split("://",url_address, protocol, after_protocol)){
			error_log("Can't convert url:"+url_address+"to CoralCDN url");
			return "";
		}
		if (protocol!="http") return "";
		string site_name_and_port, path;
		if (split("/",after_protocol,site_name_and_port,path)){
			error_log("Can't convert url:"+url_address+"to CoralCDN url");
			return "";
		}
		string site_name,site_port;
		string new_coral_cdn_url;
		if (split(":",site_name_and_port,site_name,site_port)){
			string site_name=site_name_and_port;
			new_coral_cdn_url=protocol+"://"+site_name+".nyud.net/"+path;
		}else{;
			new_coral_cdn_url=protocol+"://"+site_name+"."+site_port+".nyud.net/"+path;
		}
		return new_coral_cdn_url;
	}catch(...){
		error_log("Error in mirror.cpp: convert_to_coral_cdn_url()");
	}
	return "";
}

double Tmirror::mirror_on_the_wall(){
	try{
		double criterion;
		if (dld_size>0){
			criterion=honesty*1000000000*dld_time/dld_size;
		}else{
			criterion=honesty*1000000000*dld_time;
		}
		honesty=honesty*100/(100+settings.benchmark_oblivion);
		return criterion;
	}catch(...){
		error_log("Error in mirror.cpp: mirror_on_the_wall()");
		return 0;
	}
}
void Tmirror::start(){
	try{
		if (!dld_time)
			dld_time=1;
		active_num++;
		honesty=1;
	}catch(...){
		error_log("Error in mirror.cpp: start()");
	}
}

void Tmirror::stop(ulong time, ulong size){
	try{
		if (size==0){
			failed_downloads++;
		}else{
			successful_downloads++;
		};

		dld_time+=time/1000;
		dld_size+=size;
		honesty=1;
		debug("Connnection to Mirror stopped: Time spent:"+toString(time)+"--- Size downloaded:"+toString(size));
		active_num--;
	}catch(...){
		error_log("Error in mirror.cpp: stop()");
	}
}

Tmirror* find_mirror(string mirror_url){
	try{
		map<string,Tmirror*>::const_iterator mirror_iterator = mirror_list.find(mirror_url);
		if (mirror_iterator==mirror_list.end()){
			Tmirror * Pnew_mirror=new Tmirror;
			debug("Cant find mirror:"+mirror_url+" - creating new record");
			mirror_list[mirror_url]=Pnew_mirror;
			return Pnew_mirror;
		}
		else{
			debug("Found mirror:"+mirror_url);
			debug("==================>>");
			debug("       time:"+toString((uint)mirror_iterator->second->dld_time));
			debug("       size:"+toString(mirror_iterator->second->dld_size));
			debug("    honesty:"+toString((uint)mirror_iterator->second->honesty));
			debug("  criterion:"+toString((uint)mirror_iterator->second->mirror_on_the_wall()));
			return mirror_iterator->second;
		}
	}catch(...){
		error_log("Error in mirror.cpp: find_mirror()");
		return 0;
	}
}