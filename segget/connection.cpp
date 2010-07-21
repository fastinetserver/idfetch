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
#include "connection.h"

uint Tconnection::total_connections=0;
Tconnection connection_array[MAX_CONNECTS];
time_t prev_time;

void init_connections(){
	for (ulong connection_num=0; connection_num<MAX_CONNECTS; connection_num++){
		connection_array[connection_num].connection_num=connection_num;
	};
}

void Tconnection::start(CURLM *cm, uint network_number, uint distfile_num, Tsegment *started_segment, uint best_mirror_num){
	try{
		stats.active_connections_counter++;
		segment=started_segment;
		debug("Starting connection for distfile: "+segment->parent_distfile->name);
		mirror_num=best_mirror_num;
		network_num=network_number;
		total_dld_bytes=0;
		bytes_per_last_interval=0;
		gettimeofday(&start_time,NULL);
		active=true;
		debug("Connecting network"+toString(network_num));

		segment->parent_distfile->active_connections_num++;

		if (network_array[network_num].network_mode==MODE_PROXY_FETCHER){
			connection_start_time_network_phase_for_pf_networks=segment->parent_distfile->network_distfile_brokers_array[network_num].phase;
		}

		Tmirror *Pcurr_mirror;
		string url;
		if (network_array[network_num].network_mode==MODE_REMOTE){
			url=segment->parent_distfile->url_list[mirror_num];
			Pcurr_mirror=find_mirror(strip_mirror_name(url));
		}else{
			Pcurr_mirror=&network_array[network_num].benchmarked_mirror_list[mirror_num];
			url=Pcurr_mirror->url+segment->parent_distfile->name;
		}
		debug("  URL:"+url);

		debug("aaaaa");
		Pcurr_mirror->start();
		debug("bbbbb");
		network_array[network_num].connect();
		debug("ccccc");
		segment->prepare_for_connection(cm, connection_num, network_num, distfile_num, url);
		debug("Started connection for distfile: "+segment->parent_distfile->name);
	}catch(...){
		error_log("Error in connection.cpp: start()");
	}
}
/*
string explain_curl_error(int error_code){
	try{
		//curl_easy_strerror(
	}catch(...){
		error_log("Error in connection.cpp: explain_curl_error()");
	}
	return "Error in connection.cpp: explain_curl_error()";
}
*/
void Tconnection::stop(CURLcode connection_result){
	try{
		stats.active_connections_counter--;
		debug("Finished connection for distfile: "+segment->parent_distfile->name+" Segment#:"+toString(segment->segment_num)+" Network#"+toString(network_num)+" Status: "+toString(connection_result));
		if (connection_result){
			string error_str=curl_easy_strerror(connection_result);
			debug("	ERROR "+toString(connection_result)+": "+error_str);
			error_log("Finished connection for distfile: "+segment->parent_distfile->name+" Segment#:"+toString(segment->segment_num)+" Network#"+toString(network_num)+" Status: "+toString(connection_result));
			error_log("	ERROR "+toString(connection_result)+": "+error_str);
		}
		
		msg_clean_connection(connection_num);
		active=false;
		network_array[network_num].disconnect();
//		network_array[network_num].benchmarked_mirror_list[mirror_num].stop();
		segment->segment_file.close();
		if (connection_result==0){
			if (! segment->segment_verification_is_ok()){
				debug("curl_lies - there is a problem downloading segment");
				error_log("curl_lies - there is a problem downloading segment");
				connection_result=CURLE_READ_ERROR;
			}
		}

		segment->parent_distfile->active_connections_num--;
/*
		Tmirror *Pcurr_mirror;
		if (network_array[network_num].network_mode==MODE_LOCAL){
			Pcurr_mirror=&network_array[network_num].benchmarked_mirror_list[mirror_num];
			prnt_distfile->network_distfile_brokers_array[network_num].mirror_fails_vector[mirror_num]=true;
//			find_mirror(strip_mirror_name(segment->url));
		}else{
			Pcurr_mirror=find_mirror(strip_mirror_name(segment->url));
		}
*/
		
		Tmirror *Pcurr_mirror;
		if (network_array[network_num].network_mode==MODE_REMOTE){
			Pcurr_mirror=find_mirror(strip_mirror_name(segment->url));
		}else{
			Pcurr_mirror=&network_array[network_num].benchmarked_mirror_list[mirror_num];
		}

		timeval now_time;
		gettimeofday(&now_time,NULL);

		if (connection_result!=0){
			switch (network_array[network_num].network_mode){
				case MODE_LOCAL:{
	//				prnt_distfile->network_distfile_brokers_array[network_num].mirror_fails_vector[mirror_num]=true;
					segment->parent_distfile->network_distfile_brokers_array[network_num].local_mirror_failed(mirror_num);
	//				find_mirror(strip_mirror_name(segment->url));
					break;
				}
				case MODE_PROXY_FETCHER:{
	//				prnt_distfile->network_distfile_brokers_array[network_num].mirror_fails_vector[mirror_num]=true;
					if (connection_start_time_network_phase_for_pf_networks==E_USE_AS_LOCAL_MIRRORS){
						segment->parent_distfile->network_distfile_brokers_array[network_num].local_mirror_failed(mirror_num);
					}else{ // proxy-fetcher mirror failed, if everything correct it must be in phase E_PROXY_FETCHER_DOWNLOADED,
						segment->parent_distfile->network_distfile_brokers_array[network_num].proxy_fetcher_mirror_failed(mirror_num);
					}
	//				find_mirror(strip_mirror_name(segment->url));
					break;
				}
				default:{ // MODE REMOTE MIRRORS
				}
			}
			// error -> start downloading again
			msg_status2(segment->connection_num, toString(connection_result)+"]- Failed download "+segment->file_name);
			debug(toString(connection_result)+"]- Failed download "+segment->url);
			Pcurr_mirror->stop(time_left_from(connection_array[connection_num].start_time),0);
			if (segment->try_num>=settings.max_tries){
				segment->status=SFAILED;
				segment->parent_distfile->status=DFAILED;
				error_log("Segget failed to download distfile: "+segment->parent_distfile->name);
				error_log("Segment:"+segment->file_name+" has reached max_tries limit - segment.status set to FAILED");
			}
			else segment->status=SWAITING;
		}else{
			// no error => count this one and start new
			log("Succesfully downloaded "+segment->file_name+" on connection#"+toString(connection_num));
			debug(" Successful download "+segment->url);
// already done earlier in this function			Pcurr_mirror=find_mirror(strip_mirror_name(segment->url));
			Pcurr_mirror->stop(time_left_from(connection_array[connection_num].start_time),segment->segment_size);
			segment->status=SDOWNLOADED;
			segment->parent_distfile->inc_dld_segments_count(segment);
		};
	}catch(...){
		error_log("Error in connection.cpp: stop()");
	}
}

void Tconnection::inc_bytes_per_last_interval(ulong new_bytes_count){
	try{
		total_dld_bytes+=new_bytes_count;
		bytes_per_last_interval+=new_bytes_count;
	}catch(...){
		error_log("Error in connection.cpp: inc_bytes_per_last_interval()");
	}
}

void Tconnection::show_connection_progress(ulong time_diff){
	try{
		stats.total_bytes_per_last_interval+=bytes_per_last_interval;
		msg_segment_progress(
			segment->connection_num,
			network_num,
			segment->segment_num,
			segment->try_num,
			segment->downloaded_bytes,
			segment->segment_size,
			(bytes_per_last_interval*1000)/time_diff,
			(total_dld_bytes*1000)/time_left_from(start_time));
		bytes_per_last_interval=0;
	}catch(...){
		error_log("Error in connection.cpp: show_connection_progress()");
	}
}