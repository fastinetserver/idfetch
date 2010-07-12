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

#include "stats.h"

void Tstats::inc_dld_distfiles_count(){
	try{
		dld_distfiles_count++;
		if ((settings.del_pkg_list_when_dld_finished) and (dld_distfiles_count>distfiles_count)){
			//delete pkg.list file;
			if(remove((settings.pkg_list_dir+"/pkg.list").c_str()) != 0 )
				error_log("Error in stats.cpp: inc_dld_distfiles_count(): Can't delete:"+settings.pkg_list_dir+"/pkg.list");
			else
				debug(settings.pkg_list_dir+"/pkg.list"+" deleted" );
		}
	}catch(...){
		error_log("Error in stats.cpp: show_totals()");
	}
}

void Tstats::show_totals(){
	try{
		ulong show_last_time_interval=1;
		ulong show_total_size=1;
		if (last_time_interval>1)
			show_last_time_interval=last_time_interval;
		if (total_size>1)
			show_total_size=total_size;
		struct timeval now_timee;
		gettimeofday(&now_timee,NULL);
		msg_total("Total" 
			+field(" PKGs:",          pkg_count,4)
			+field(" = DFs:",         dld_distfiles_count,4)
			+field(" / ",             distfiles_count,4)
			+field(" = Size:",        dld_size/1000,7)
			+field(" / ",             total_size/1000,7)+" Kb "
			+field(" = ",             dld_size*100/(1+show_total_size),3)+"%"
			+field(" Total speed: ",  (total_bytes_per_last_interval/(show_last_time_interval)),7)
			+" Kb/s"
//			+" Secs:"+toString(now_timee.tv_sec)
//			+" usecs:"+toString(now_timee.tv_usec)
			);
		reset_previous_time();
	}catch(...){
		error_log("Error in stats.cpp: show_totals()");
	}
}

void Tstats::reset_previous_time(){
	try{
		gettimeofday(&previous_time,NULL);
	}catch(...){
		error_log("Error in stats.cpp: reset_previous_time()");
	}
}