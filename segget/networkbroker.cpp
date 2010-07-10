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
void Tnetwork_distfile_broker::init(ulong network_number){
	try{
		network_num=network_number;
		if (network_array[network_num].network_mode==MODE_LOCAL){
			// create flags for each mirror from this network
			mirror_fails_vector.insert(mirror_fails_vector.begin(),network_array[network_num].benchmarked_mirror_list.size(),false);
		}
	}catch(...){
		error_log("Error in networkbroker.cpp: init()");
	}
}

bool Tnetwork_distfile_broker::some_mirrors_have_NOT_failed_yet(){
	try{
		for (ulong cur_mirror_num=0; cur_mirror_num<mirror_fails_vector.size();cur_mirror_num++){
			if (! mirror_fails_vector[cur_mirror_num]){
				return true;
			}
		}
		return false;
	}catch(...){
		error_log("Error in networkbroker.cpp: init()");
		return false;
	}
}