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

#include "segment.h"

Tsegment *segments_in_progress[MAX_CONNECTS]={0};

string statusToString(Tsegment_status the_status){
	switch (the_status){
		case SWAITING:return "SWAITING";
		case SDOWNLOADING:return "SDOWNLOADING";
		case SDOWNLOADED:return "SDOWNLOADED";
		case SFAILED:return "SFAILED";
		default :return "UNKNOWN STATUS";
	}
}
void Tsegment::set_segment(Tdistfile *prnt_distfile, uint seg_num, string distfile_name, ulong default_seg_size, ulong range_end){
	try{
		parent_distfile=prnt_distfile;
		segment_num=seg_num;
		segment_size=range_end-seg_num*default_seg_size+1;
		range=toString(seg_num*default_seg_size)+"-"+toString(range_end);
		file_name="."+distfile_name+".seg"+toString(seg_num);
		ulong downloaded_size;
		if (settings.get_resume()){
			//check if downloaded
			ifstream file((settings.segments_dir+"/"+file_name).c_str(), ios::in|ios::binary);
			ulong start = file.tellg();
			file.seekg (0, ios::end);
			ulong end = file.tellg();
			downloaded_size = end - start;
			debug("seg:"+toString(seg_num)+" Dsize="+toString(downloaded_size)+" seg_size="+toString(segment_size));
			file.close();
			if (downloaded_size==segment_size){
				status=SDOWNLOADED;
				debug("seg:"+toString(seg_num)+" Downloaded");
			}
			else{
				debug("seg:"+toString(seg_num)+" not downloaded");
				downloaded_bytes=0;
			}
		}
	}catch(...){
		error_log("Error in segment.cpp: prepare_for_connection()");
	}
}
void Tsegment::prepare_for_connection(CURLM *cm, uint con_num, uint network_num, uint distfile_num, uint mirror_num){
	try{
//		debug("NETWORK:"+toString(network_num)+(network_array[network_num].use_own_mirror_list_only_on ? " - LOCAL": " - REMOTE"));
		if (network_array[network_num].network_mode==MODE_LOCAL){
			url=network_array[network_num].benchmarked_mirror_list[mirror_num].url+parent_distfile->name;
			debug("  URL:"+url);
		}else{
			url=parent_distfile->url_list[mirror_num];
		}
		msg_connecting(con_num,distfile_num, segment_num,"Downloading from "+url);
		segments_in_progress[con_num]=this;
		status=SDOWNLOADING;
		downloaded_bytes=0;
		connection_num=con_num;
//		connection_array[con_num].start(network_num);
		try_num++;
		add_easy_handle_to_multi(cm, network_num);
	}catch(...){
		error_log("Error in segment.cpp: prepare_for_connection()");
	}
}

Tsegment::~Tsegment(){
	try{
		segment_file.close();
	}catch(...){
		error_log("Error in segment.cpp: ~Tsegment()");
	}
}

int Tsegment::add_easy_handle_to_multi(CURLM *cm, uint network_num){
	try{
		segment_file.exceptions (ofstream::badbit);
		segment_file.open((settings.segments_dir+"/"+file_name).c_str(), ios::trunc|ios::binary );
	}catch(std::ifstream::failure e){
			if (!segment_file.eof()){
					error_log("Can't open segment file:"+settings.segments_dir+"/"+file_name+": "+(string)e.what());
				return 1;
			}
	}catch(...){
		error_log("Unknown error: Can't open segment file:"+settings.segments_dir+"/"+file_name);
		return 1;
	}
	try{
		easyhandle = curl_easy_init();
		if(easyhandle) {
			curl_easy_setopt(easyhandle, CURLOPT_URL, url.c_str() );
			curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, this);
			curl_easy_setopt(easyhandle, CURLOPT_PRIVATE, this);
			curl_easy_setopt(easyhandle, CURLOPT_RANGE, range.c_str());
			curl_easy_setopt(easyhandle, CURLOPT_TIMEOUT, network_array[network_num].time_out);
			curl_easy_setopt(easyhandle, CURLOPT_FTP_RESPONSE_TIMEOUT, network_array[network_num].ftp_response_timeout);
			if ((network_array[network_num].low_connection_speed_limit!=0) and (network_array[network_num].low_connection_speed_limit!=(ulong)-1)){
				curl_easy_setopt(easyhandle, CURLOPT_LOW_SPEED_LIMIT, network_array[network_num].low_connection_speed_limit);
			}
			curl_easy_setopt(easyhandle, CURLOPT_LOW_SPEED_TIME, network_array[network_num].low_connection_speed_time);

			if ((network_array[network_num].max_connection_speed!=0) and (network_array[network_num].max_connection_speed!=(ulong)-1)){
				curl_easy_setopt(easyhandle, CURLOPT_MAX_RECV_SPEED_LARGE, network_array[network_num].max_connection_speed);
			}
			curl_easy_setopt(easyhandle, CURLOPT_USERAGENT, network_array[network_num].user_agent.c_str());

			if (network_array[network_num].proxy_off)
				curl_easy_setopt(easyhandle, CURLOPT_NOPROXY, "*");
			else{
				if ((network_array[network_num].proxy_ip_or_name!="none") and (network_array[network_num].proxy_port!=0)){
					curl_easy_setopt(easyhandle, CURLOPT_PROXY, network_array[network_num].proxy_ip_or_name.c_str());
					curl_easy_setopt(easyhandle, CURLOPT_PROXYPORT, network_array[network_num].proxy_port);
					debug("Using proxy:"+network_array[network_num].proxy_ip_or_name+":"+toString(network_array[network_num].proxy_port));
				}
				if (network_array[network_num].proxy_user!="none"){
					curl_easy_setopt(easyhandle, CURLOPT_PROXYUSERNAME, network_array[network_num].proxy_user.c_str());
					if (network_array[network_num].proxy_password!="none")
						curl_easy_setopt(easyhandle, CURLOPT_PROXYPASSWORD, network_array[network_num].proxy_password.c_str());
				}
			}

			if ((network_array[network_num].bind_interface!="none") 
					and (network_array[network_num].bind_interface!=""))
				curl_easy_setopt(easyhandle, CURLOPT_INTERFACE, network_array[network_num].bind_interface.c_str());
			//set connection timeout
			curl_easy_setopt(easyhandle, CURLOPT_CONNECTTIMEOUT, network_array[network_num].connection_timeout);
			curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, write_data);
			curl_multi_add_handle(cm, easyhandle);
			return 0;
		}else
			return 1;
	}catch(...){
		error_log("Error in segment.cpp: add_easy_handle_to_multi()");
		return 1;
	}
}
void show_progress(double time_diff){
	try{
		stats.total_bytes_per_last_interval=0;
		for (uint con_num=0; con_num<MAX_CONNECTS; con_num++){
			//    ulong speed=bytes_written*1000/(diff_sec+diff_milli);
			//if connection is not NULL
			connection_array[con_num].show_connection_progress(time_diff);
		}
		stats.last_time_interval=time_diff;
		stats.show_totals();
	}
	catch(...){
		error_log("Error in segment.cpp: show_progress()");
	}
}

size_t write_data(void *buffer, size_t size, size_t nmemb, void *cur_segment){
	ulong bytes_written=size*nmemb;
	try{
		Tsegment *segment;
		segment =(Tsegment*)cur_segment;
		segment->downloaded_bytes+=nmemb;
		try{
			segment->segment_file.write((char*)buffer,nmemb*size);
		}
		catch(...){
			error_log("Can't write segment file:"+segment->file_name);
		}
		connection_array[segment->connection_num].inc_bytes_per_last_interval(bytes_written);
//		ulong time_diff_msecs=time_left_from(stats.previous_time);
//		if (time_diff_msecs >= settings.current_speed_time_interval_msecs){
//			show_progress(time_diff_msecs);
//		};
	}catch(...){
		error_log("Error in segment.cpp: write_data()");
		return 0;
	}
	return bytes_written;
}