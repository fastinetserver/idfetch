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

#include "tuidistfile.h"
#include "log.h"

vector <Ttuidistfile> tuidistfiles;

string Ttuidistfile::statusToString(){
	try{
		switch(status){
			case DNEW: return "Added";
			case D_NOT_PROXY_REQUESTED: return "Not proxy requested";
			case DPROXY_REJECTED: return "Proxy rejected";
			case DPROXY_QUEUED: return "Proxy queued";
			case DPROXY_DOWNLOADING: return "Downloading via proxy";
			case DPROXY_DOWNLOADED: return "Downloaded via proxy";
			case DPROXY_FAILED: return "Failed";
			case DALL_LM_AND_PF_MIRRORS_FAILED: return "All mirrors failed";
			case DWAITING: return "Waiting";
			case DSCRIPTREJECTED: return "Rejected by script";
			case DDOWNLOADING: return "Downloading";
			case DDOWNLOADED: return "Downloaded";
			case DFAILED: return "Failed";
		}
	}catch(...){
		error_log("Error: distfile.cpp: statusToString()");
	}
	return "UNKNOWN status";
}

long find_distfile_by_name(string distfile_name){
	try{
		for (ulong cur_distfile=0;cur_distfile<tuidistfiles.size();cur_distfile++)
			if (tuidistfiles[cur_distfile].name==distfile_name) return cur_distfile;
	}catch(...){
		perror("Error in tuidistfile.cpp: find_distfile_by_name()");
	}
	return -1;
}


