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
#include "network.h"

string phaseToString(int phase_num);



void Tnetwork_distfile_broker::init(ulong network_number){
	try{
		network_num=network_number;
		// init mirror_fail_vector for LOCAL AND PROXY-FETCHER NETWORK BUT NOT REMOTE NETWORK
		if (network_array[network_num].network_mode!=MODE_REMOTE){
			// create flags for each mirror from this network
			mirror_fails_vector.insert(mirror_fails_vector.begin(),network_array[network_num].benchmarked_mirror_list.size(),false);
			gettimeofday(&last_request_time, NULL);
		}
	}catch(...){
		error_log("Error in networkbroker.cpp: init()");
	}
}

void Tnetwork_distfile_broker::local_mirror_failed(uint mirror_num){
	try{
		if (phase==E_USE_AS_LOCAL_MIRRORS){
			if (! mirror_fails_vector[mirror_num]){
				mirror_fails_vector[mirror_num]=true;
				failed_mirrors_num++;
			}
			if (failed_mirrors_num>=mirror_fails_vector.size()){
				phase=E_ALL_LOCAL_MIRRORS_FAILED;
				failed_mirrors_num=0;
				// clean the vector, to use it again when we switch to E_PROXY_FETCHER_DOWNLOADED phase
				for (ulong mirr_num=0; mirr_num<mirror_fails_vector.size(); mirr_num++){
					mirror_fails_vector[mirr_num]=false;
				}
			}
		}
	}catch(...){
		error_log("Error in networkbroker.cpp: local_mirror_failed()");
	}
}

void Tnetwork_distfile_broker::proxy_fetcher_mirror_failed(uint mirror_num){
	try{
		if (phase==E_PROXY_FETCHER_DOWNLOADED){
			if (! mirror_fails_vector[mirror_num]){
				mirror_fails_vector[mirror_num]=true;
				failed_mirrors_num++;
			}
			if (failed_mirrors_num>=mirror_fails_vector.size()){
				phase=E_ALL_PROXY_FETCHER_MIRRORS_FAILED;
				failed_mirrors_num=0;
			}
		}
	}catch(...){
		error_log("Error in networkbroker.cpp: proxy_fetcher_mirror_failed()");
	}
}