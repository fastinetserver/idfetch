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

void Tconnection::start(CURLM *cm, uint network_number, uint distfile_num, Tsegment *started_segment, uint best_mirror_num){
	try{
		segment=started_segment;
		mirror_num=best_mirror_num;
		network_num=network_number;
		total_dld_bytes=0;
		bytes_per_last_interval=0;
		gettimeofday(&start_time,NULL);
		active=true;
		network_array[network_num].connect();
		segment->prepare_for_connection(cm, connection_num, network_num, distfile_num, mirror_num);
	}catch(...){
		error_log("Error in connection.cpp: start()");
	}
}

void Tconnection::stop(uint connection_result){
	try{
		msg_clean_connection(connection_num);
		active=false;
		network_array[network_num].disconnect();
		segment->segment_file.close();

		Tdistfile* prnt_distfile=segment->parent_distfile;
		prnt_distfile->active_connections_num--;

		Tmirror *Pcurr_mirror;
		if (network_array[network_num].use_own_mirror_list_only_on){
			Pcurr_mirror=&network_array[network_num].benchmarked_mirror_list[mirror_num];
			prnt_distfile->network_distfile_brokers_array[network_num].mirror_fails_vector[mirror_num]=true;
//			find_mirror(strip_mirror_name(segment->url));
		}else{
			Pcurr_mirror=find_mirror(strip_mirror_name(segment->url));
		}

		timeval now_time;
		gettimeofday(&now_time,NULL);

		if (connection_result!=0){
			// error -> start downloading again
			msg_status2(segment->connection_num, toString(connection_result)+"]- Failed download "+segment->file_name);
			debug(toString(connection_result)+"]- Failed download "+segment->url);
			Pcurr_mirror->stop(time_left_from(connection_array[connection_num].start_time),0);
			if (segment->try_num>=settings.max_tries){
				segment->status=FAILED;
				error_log("Segment:"+segment->file_name+" has reached max_tries limit - segment.status set to FAILED");
			}
			else segment->status=WAITING;
		}else{
			// no error => count this one and start new
			log("Succesfully downloaded "+segment->file_name+" on connection#"+toString(connection_num));
			debug(" Successful download "+segment->url);
			Pcurr_mirror->stop(time_left_from(connection_array[connection_num].start_time),segment->segment_size);
			segment->status=DOWNLOADED;
			prnt_distfile->inc_dld_segments_count(segment);
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
		if (active){
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
		}
	}catch(...){
		error_log("Error in connection.cpp: show_connection_progress()");
	}
}