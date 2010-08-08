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
#include "utils.h"
#include "settings.h"

long script_waiting_connection_num=-1;
uint Tconnection::total_connections=0;
Tconnection connection_array[MAX_CONNECTS];

void init_connections(){
	for (ulong connection_num=0; connection_num<MAX_CONNECTS; connection_num++){
		connection_array[connection_num].connection_num=connection_num;
	};
}

int Tconnection::start(CURLM *cm, uint network_number, uint distfile_num, Tsegment *started_segment, uint best_mirror_num){
	try{
		segment=started_segment;
		debug("Starting connection for distfile: "+segment->parent_distfile->name);
		mirror_num=best_mirror_num;
		network_num=network_number;
		total_dld_bytes=0;
		bytes_per_last_interval=0;
		gettimeofday(&start_time,NULL);
		debug("Connecting network"+toString(network_num));

		segment->parent_distfile->set_status(DDOWNLOADING);

		if (network_array[network_num].network_mode==MODE_PROXY_FETCHER){
			connection_start_time_network_phase_for_pf_networks=segment->parent_distfile->network_distfile_brokers_array[network_num].phase;
		}

		Tmirror *Pcurr_mirror;
		switch (network_array[network_num].network_mode){
			case MODE_REMOTE:{
				url=segment->parent_distfile->url_list[mirror_num];
				Pcurr_mirror=find_mirror(strip_mirror_name(url));
				break;
			}
			case MODE_CORAL_CDN:{
				url=convert_to_coral_cdn_url(segment->parent_distfile->url_list[mirror_num]);
				Pcurr_mirror=find_mirror(strip_mirror_name(url));
				break;
			}
			default:{
				Pcurr_mirror=&network_array[network_num].benchmarked_mirror_list[mirror_num];
				url=Pcurr_mirror->url+segment->parent_distfile->name;
			}
		}
		debug("  URL:"+url);

		if (run_user_python_script(connection_num)){
			if (segment->parent_distfile->active_connections_num<=0){
				segment->parent_distfile->set_status(DSCRIPTREJECTED);
			}
			return REJECTED_BY_USER_PYTHON_SCRIPT;
		}

		segment->parent_distfile->active_connections_num++;
		active=true;
		Pcurr_mirror->start();
		network_array[network_num].connect();

		stats.active_connections_counter++;
		segment->prepare_for_connection(cm, connection_num, network_num, distfile_num, url);
		debug("Started connection for distfile: "+segment->parent_distfile->name);
		return 0;
	}catch(...){
		error_log("Error in connection.cpp: start()");
	}
	return ERROR_WHILE_PREPARING_CONNECTION;
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
		Tmirror *Pcurr_mirror;
		switch (network_array[network_num].network_mode){
			case MODE_REMOTE:
			case MODE_CORAL_CDN:{
				Pcurr_mirror=find_mirror(strip_mirror_name(url));
				break;
			}
			default:{
				Pcurr_mirror=&network_array[network_num].benchmarked_mirror_list[mirror_num];
			}
		}
		debug("Finished connection for distfile: "+segment->parent_distfile->name+" Segment#:"+toString(segment->segment_num)+" Network#"+toString(network_num)+" Status: "+toString(connection_result));
		if (connection_result){
			string error_str=curl_easy_strerror(connection_result);
			debug("  ERROR "+toString(connection_result)+": "+error_str);
			error_log("Finished connection for distfile: "+segment->parent_distfile->name+" Segment#:"+toString(segment->segment_num)+" Network#"+toString(network_num)+" Status: "+toString(connection_result));
			error_log("  ERROR "+toString(connection_result)+": "+error_str);
		}
		active=false;
		network_array[network_num].disconnect();
//		network_array[network_num].benchmarked_mirror_list[mirror_num].stop();
		segment->segment_file.close();
		if (connection_result==0){
			if (! segment->segment_verification_is_ok()){
				connection_result=CURLE_READ_ERROR;
				Pcurr_mirror->stop(time_left_since(connection_array[connection_num].start_time),0);
				debug("curl_lies - there is a problem downloading segment");
				error_log("curl_lies - there is a problem downloading segment");
			}else{
				Pcurr_mirror->stop(time_left_since(connection_array[connection_num].start_time),segment->segment_size);
			}
		}else{
			Pcurr_mirror->stop(time_left_since(connection_array[connection_num].start_time),0);
		}
		segment->parent_distfile->active_connections_num--;

		msg_clean_connection(connection_num);

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
		debug("before gettimeofday");
		timeval now_time;
		gettimeofday(&now_time,NULL);

		if (connection_result!=0){
			stats.fails_counter++;
			switch (network_array[network_num].network_mode){
				case MODE_LOCAL:{
	//				prnt_distfile->network_distfile_brokers_array[network_num].mirror_fails_vector[mirror_num]=true;
					debug("before setting mirror fail");
					segment->parent_distfile->network_distfile_brokers_array[network_num].local_mirror_failed(mirror_num);
					debug("after setting mirror fail");
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
//			msg_status2(segment->connection_num, toString(connection_result)+"]- Failed download "+segment->file_name);
			debug(toString(connection_result)+"]- Failed download "+segment->url);
			if (segment->try_num>=settings.max_tries){
				segment->status=SFAILED;
				segment->parent_distfile->set_status(DFAILED);
				error_log("Segget failed to download distfile: "+segment->parent_distfile->name);
				error_log("Segment:"+segment->file_name+" has reached max_tries limit - segment.status set to FAILED");
			}
			else segment->status=SWAITING;
		}else{
			// no error => count this one and start new
			log("Succesfully downloaded "+segment->file_name+" on connection#"+toString(connection_num));
			debug(" Successful download "+segment->url);
// already done earlier in this function			Pcurr_mirror=find_mirror(strip_mirror_name(segment->url));
			segment->status=SDOWNLOADED;
			segment->parent_distfile->inc_dld_segments_count(segment);
		};
		if ((segment->parent_distfile->get_status()!=DFAILED)
		and (segment->parent_distfile->get_status()!=DDOWNLOADED)
		and (segment->parent_distfile->get_status()!=DALL_LM_AND_PF_MIRRORS_FAILED)){
			if (segment->parent_distfile->active_connections_num>0){
				segment->parent_distfile->set_status(DDOWNLOADING);
			}else{
				segment->parent_distfile->set_status(DWAITING);
			}
		}
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
		ulong speed=(bytes_per_last_interval*1000)/time_diff;
		ulong avg_speed=(total_dld_bytes*1000)/time_left_since(start_time);
		string eta_string;
		if (avg_speed==0){
			eta_string=" ETA: inf";
		}else{
			eta_string=" ETA: "+secsToString((segment->segment_size-segment->downloaded_bytes)/avg_speed);
		}
		string speed_str;
		string avg_speed_str;
		speed_str=" Speed: "+speedToString(speed);
		avg_speed_str=" AVG speed: "+speedToString(avg_speed);
		int percent=segment->downloaded_bytes*100/segment->segment_size;
		string network_type_str;
		switch (network_array[network_num].network_mode){
			case MODE_REMOTE: network_type_str="REM"; break;
			case MODE_PROXY_FETCHER: network_type_str="P-F"; break;
			case MODE_LOCAL: network_type_str="LOC"; break;
			case MODE_CORAL_CDN: network_type_str="CDN"; break;
		}
		string progress_text=field("[Net",network_num,1)
			+":"+network_type_str+"]"
			+field(" Segment:",segment->segment_num, 5)
			+field(" Try:",segment->try_num,4)
			+field(" Bytes:",segment->downloaded_bytes,7)
			+field(" / ",segment->segment_size,7)
			+field(" = ",percent,3)+"%"
			+speed_str
			+avg_speed_str
			+eta_string;
		msg_segment_progress(segment->connection_num, progress_text);
		bytes_per_last_interval=0;
	}catch(...){
		error_log("Error in connection.cpp: show_connection_progress()");
	}
}

string Tconnection::get_html_connection_progress(){
	try{
		ulong time_diff=time_left_since(stats.previous_time);
//		stats.total_bytes_per_last_interval+=bytes_per_last_interval;
		ulong speed=(bytes_per_last_interval*1000)/time_diff;
		ulong avg_speed=(total_dld_bytes*1000)/time_left_since(start_time);
		string eta_string;
		if (avg_speed==0){
			eta_string="inf";
		}else{
			eta_string=secsToString((segment->segment_size-segment->downloaded_bytes)/avg_speed);
		}
		string speed_str;
		string avg_speed_str;
		speed_str=speedToString(speed);
		avg_speed_str=speedToString(avg_speed);
		int segment_percent;
		if (segment->segment_size>0){
			segment_percent=segment->downloaded_bytes*100/segment->segment_size;
		}else{
			segment_percent=100;
		}
		int distfile_percent;
		if (segment->parent_distfile->size>0){
			distfile_percent=segment->parent_distfile->dld_bytes*100/segment->parent_distfile->size;
		}else{
			distfile_percent=100;
		}
		
		int unfinished_segments_distfile_percent;
		//TO-DO: it's necessary to check all connections and add dld bytes to be correct !!!
		if (segment->parent_distfile->size>0){
			unfinished_segments_distfile_percent=segment->downloaded_bytes*100/segment->parent_distfile->size;
		}else{
			unfinished_segments_distfile_percent=0;
		}
		string network_type_str;
		switch (network_array[network_num].network_mode){
			case MODE_REMOTE: network_type_str="Remote"; break;
			case MODE_PROXY_FETCHER: network_type_str="Proxy-Fetcher"; break;
			case MODE_LOCAL: network_type_str="Local"; break;
			case MODE_CORAL_CDN: network_type_str="CDN"; break;
		}
		string progress_text=
			(string)"<td align=center><table width=100 border=0 cellpadding=0 frame=void><tr>"
				+((distfile_percent>0)?"<td bgcolor=\"#0000FF\"><img src=\"/img/blue_progress.jpg\" height=20 width="+toString(distfile_percent)+"/></td>"
					:"")
				+((unfinished_segments_distfile_percent>0)?"<td bgcolor=\"#00FF00\"><img src=\"/img/green_progress.jpg\" height=20 width="+toString(unfinished_segments_distfile_percent)+"/></td>"
					:"")
				+((100-distfile_percent-unfinished_segments_distfile_percent>0)?"<td bgcolor=\"#555555\"><img src=\"/img/bw_progress.jpg\" height=20 width="+toString(100-distfile_percent-unfinished_segments_distfile_percent)+" /></td>"
					:"")
			+"</tr></table>"
//			+"&nbsp;"+toString(distfile_percent+unfinished_segments_distfile_percent)+"%"
			+"&nbsp;"+toString(distfile_percent)+"%"
			+"</td><td>"
				+"<table>"
					+"<tr><td>"+segment->parent_distfile->name+"</td></tr>"
					+"<tr><td>"+segment->url+"</td></tr>"
				+"</table>"
			+"</td><td align=center><table width=100 border=0 cellpadding=0 frame=void><tr>"
				+((segment_percent>0)?"<td bgcolor=\"#00FF00\"><img src=\"/img/green_progress.jpg\" height=20 width="+toString(segment_percent)+"/></td>"
					:"")
				+((100-segment_percent>0)?"<td bgcolor=\"#555555\"><img src=\"/img/bw_progress.jpg\" height=20 width="+toString(100-segment_percent)+" /></td>"
					:"")
				+"</tr></table>"
			+"&nbsp;"+toString(segment_percent)+"%"
			+"</td><td align=right>"+toString(segment->segment_num)
			+"</td><td align=right>"+toString(segment->try_num)
			+"</td><td align=right>"+toString(network_num)
			+"</td><td align=center>"+network_type_str
			+"</td><td align=right>"+toString(segment->downloaded_bytes)
			+"</td><td align=right>"+toString(segment->segment_size)
			+"</td><td align=right>"+speed_str
			+"</td><td align=right>"+avg_speed_str
			+"</td><td align=right>"+eta_string
			+"</td>";
		return progress_text;
	}catch(...){
		error_log("Error in connection.cpp: show_connection_progress()");
	}
	return "";
}