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

#include "utils.h"

ulong time_left_from(timeval from_time){
	try{
		timeval now_time;
		gettimeofday(&now_time,NULL);
		ulong timeleft=(now_time.tv_sec-from_time.tv_sec)*1000+(now_time.tv_usec-from_time.tv_usec)/1000;
		if (timeleft<1)
			timeleft=1;
		return timeleft;
	}catch(...){
		error_log("Error in utils.cpp: time_left_from()");
		return 1;
	}
}

string secsToString(ulong secs){
	string result="";
	ulong mins=0;
	ulong hours=0;
	ulong days=0;
	ldiv_t time_div_spliter;
	if (secs>60){
		time_div_spliter = ldiv (secs,60);
		secs=time_div_spliter.rem;
		if (time_div_spliter.quot>60){
			time_div_spliter = ldiv (time_div_spliter.quot,60);
			mins=time_div_spliter.rem;
			if (time_div_spliter.quot>24){
				time_div_spliter = ldiv (time_div_spliter.quot,24);
				hours=time_div_spliter.rem;
				days=time_div_spliter.quot;
				if (days>999) return ("inf");
				result=result+field(" ", days,3)+"d";
			}else{
				hours=time_div_spliter.quot;
			}
			result=result+field(" ", hours,2)+"h";
		}else{
			mins=time_div_spliter.quot;
		}
		result=result+field(" ", mins,2)+"m";
	}
	result=result+field(" ", secs,2)+"s";

	return result;
}

string speedToString(ulong dld_bytes, ulong time_left){
	try{
		string speed_str;
		if (time_left==0){
			speed_str="N/a";
		}else{
			ulong speed=(dld_bytes*1000/time_left);
			string suffix;
			if (speed>1500){
				if (speed>1500000){
					suffix=" MB/s";
					speed=speed/1000000;
				}else{
					suffix=" KB/s";
					speed=speed/1000;
				}
			}else{
				suffix="  B/s";
			}
			speed_str=field("",speed,4)+suffix;
		}
		return speed_str;
	}catch(...){
		error_log("Error in utils.cpp: speedToString()");
	}
	return "";
}

string speedToString(ulong speed){
	try{
		string speed_str;
		string suffix;
		if (speed>1500){
			if (speed>1500000){
				suffix=" MB/s";
				speed=speed/1000000;
			}else{
				suffix=" KB/s";
				speed=speed/1000;
			}
		}else{
			suffix="  B/s";
		}
		speed_str=field("",speed,4)+suffix;
		return speed_str;
	}catch(...){
		error_log("Error in utils.cpp: speedToString()");
	}
	return "";
}