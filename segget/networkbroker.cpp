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

#include "networkbroker.h"

bool Tnetwork_distfile_broker::get_allowed_status(){
	debug("        get_allowed_status_for_network_num="+toString(network_num));
	return network_array[network_num].get_busy_status();
}

Tmirror* Tnetwork_distfile_broker::find_mirror(string mirror_url){
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
			debug("       time:"+toString(mirror_iterator->second->dld_time));
			debug("       size:"+toString(mirror_iterator->second->dld_size));
			debug("    honesty:"+toString(mirror_iterator->second->honesty));
			debug("  criterion:"+toString(mirror_iterator->second->mirror_on_the_wall()));
			return mirror_iterator->second;
		}
	}catch(...){
		error_log("Error in mirror.cpp: find_mirror()");
		return 0;
	}
}