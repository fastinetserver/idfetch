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

#include "distfile.h"

void Tdistfile::init(){
	for (uint network_num=0; network_num<MAX_NETWORKS; network_num++){
		networkbrokers_array[network_num].network_num=network_num;
	}
}
void Tdistfile::load_url_list(json_object* json_array_distfile_urllist){
	try{
		url_count=json_object_array_length(json_array_distfile_urllist);
		url_list= new string [url_count];
		for(uint urllist_array_item_num=0;urllist_array_item_num<url_count;urllist_array_item_num++){
			url_list[urllist_array_item_num]=json_object_get_string(json_object_array_get_idx(json_array_distfile_urllist,urllist_array_item_num));
		}
		if (url_count){
			num=++stats.distfiles_count;
			stats.inc_total_size(size);
		}
	}catch(...){
		error_log("Error: distfile.cpp: load_url_list()");
	}
}

bool Tdistfile::check_if_dld(){
	try{
		ifstream filec((settings.distfiles_dir+"/"+name).c_str());
		ulong start = filec.tellg();
		filec.seekg (0, ios::end);
		ulong end = filec.tellg();
		ulong d_size;
		d_size = end - start;
		//debug("seg:"+toString(seg_num)+" Dsize="+toString(downloaded_size)+" seg_size="+toString(segment_size));
		filec.close();
		if (d_size==size){
			downloaded=true;
			num=++stats.distfiles_count;
			stats.inc_dld_distfiles_count();
			stats.inc_dld_size(size);
			debug("Distfile:"+name+" already downloaded");
			return true;
		}
		else{
			debug("Distfile:"+name+" not downloaded");
			return false;
		}
	}catch(...){
		error_log("Error: distfile.cpp: check_if_dld()");
		return false;
	}
}

void Tdistfile::load_distfile_from_json(json_object* json_obj_distfile){
	try{
		name=json_object_get_string(json_object_object_get(json_obj_distfile,"name"));
		size=atoi(json_object_to_json_string(json_object_object_get(json_obj_distfile,"size")));
		RMD160=json_object_get_string(json_object_object_get(json_obj_distfile,"RMD160"));
		SHA1=json_object_get_string(json_object_object_get(json_obj_distfile,"SHA1"));
		SHA256=json_object_get_string(json_object_object_get(json_obj_distfile,"SHA256"));

		if (not(check_if_dld())){
			split_into_segments();
			load_url_list(json_object_object_get(json_obj_distfile,"url_list"));
		}
	}catch(...){
		error_log("Error: distfile.cpp: load_distfile_from_json()");
	}
}

void Tdistfile::split_into_segments(){
	try{
		int range_end;
		if (size>settings.max_segment_size){
			segments_count=size/settings.max_segment_size;
			// set segments to have equal size, only last segment may be somewhat large (up to segment_count bytes large)
			segment_size=size/segments_count;	  
		}
		else{
			segment_size=size;
			segments_count=1;
		};
		dn_segments = new Tsegment [segments_count];
		//none downloaded yet
		for (uint segment_num=0; segment_num<segments_count; segment_num++){
			if (segment_num == (segments_count-1)){
				range_end=size-1;
				debug("last_segment ends"+toString(range_end));
			}
			else{
				range_end=(segment_num+1)*segment_size-1;
				debug("segment_range:"+toString(range_end));
		};
		dn_segments[segment_num].set_segment(this, segment_num, name, segment_size, range_end);
		if (dn_segments[segment_num].status==DOWNLOADED)
			inc_dld_segments_count(&dn_segments[segment_num]);
		}
	}catch(...){
		error_log("Error: distfile.cpp: split_into_segments()");
	}
}

Tdistfile::~Tdistfile(){
	try{
		delete [] url_list;
	}catch(...){
		error_log("Error: distfile.cpp: ~Tdistfile()");
	}
}
bool Tdistfile::choose_best_mirror(CURLM* cm, uint connection_num, uint network_num, uint seg_num){
		Tmirror *Pcurr_mirror;
		Tmirror *Pbest_mirror=0;	// the best isn't set let's find it
		uint best_mirror_num=-1;	// the best isn't set let's find it

		ulong best_mirror_self_rating=-1;
		ulong curr_mirror_self_rating;

		for (url_num=0; url_num<url_count; url_num++){
			Pcurr_mirror=find_mirror(strip_mirror_name(url_list[url_num]));
			if (Pcurr_mirror->get_active_num()<settings.max_connections_num_per_mirror){
				curr_mirror_self_rating=Pcurr_mirror->mirror_on_the_wall();
				if (curr_mirror_self_rating<best_mirror_self_rating){
					best_mirror_num=url_num;
					best_mirror_self_rating=curr_mirror_self_rating;
					Pbest_mirror=Pcurr_mirror;
				}
				if (best_mirror_self_rating==0)
					// 0 can not be improved - it's one of the best
					break;
			}
		}
		if (Pbest_mirror){
			debug("Downloading from BEST_MIRROR:"+url_list[best_mirror_num]);
			Pbest_mirror->start();
			dn_segments[seg_num].prepare_for_connection(cm, connection_num, network_num, num, url_list[best_mirror_num]);
			connection_array[connection_num].segment=&dn_segments[seg_num];
			return 0;
		}
		else{
			error_log("Can't choose mirror for segment:"+dn_segments[seg_num].file_name);
			return 1;
		}
}

int Tdistfile::provide_segment(CURLM* cm, uint connection_num, uint seg_num){

	try{
		active_connections_num++;
		//choose network
		for (uint cur_network_priority=10; cur_network_priority>0; cur_network_priority--){
			debug("cur_network_priority="+toString(cur_network_priority));
//----------------------------------------------------------------------------------------------------------
//
//       Several criterions can be used here to choose among networks with equal priority:
// min_active_connection, min_ratio_active_to_max_connection, best_speed_accocding_to_stats, etc
// add these options to segget.conf file
//
//----------------------------------------------------------------------------------------------------------
			int best_network_num=-1;
			for (uint network_num=0; network_num<MAX_NETWORKS; network_num++){
				debug("    network_num="+toString(network_num));
				//if network priority set then it's active
				if (network_array[network_num].priority){
					if (network_array[network_num].priority==cur_network_priority){
						debug("        network_priority="+toString(network_array[network_num].priority));
						if (networkbrokers_array[network_num].get_allowed_status()){
							debug("             Allowed network#:"+toString(network_num));
							if 
								((best_network_num==-1)
								or
								(network_array[best_network_num].active_connections_num>network_array[network_num].active_connections_num)){
									best_network_num=network_num;
									debug("             Replace best network to network#:"+toString(network_num));
							}
							//work with network
						}
					}
				}
			}
			if (best_network_num!=-1){
				//best network has been found
				debug("             So best network is network#:"+toString(best_network_num));
				return choose_best_mirror(cm, connection_num, best_network_num, seg_num);
			}
		}
	}catch(...){
		error_log("Error: distfile.cpp: provide_segment()");
		return 1;
	}
	return 0;
}
void Tdistfile::inc_dld_segments_count(Tsegment* current_segment){
	try{
		stats.inc_dld_size(current_segment->segment_size);
		if (++dld_segments_count==segments_count)
			combine_segments();
	}catch(...){
		error_log("Error: distfile.cpp: inc_dld_segments_count()");
	}
}
int Tdistfile::combine_segments(){
	try{
		debug("Combining distfile"+name);
		ofstream distfile_file;
		distfile_file.exceptions (ofstream::failbit | ofstream::badbit);
		try{
			distfile_file.open((settings.distfiles_dir+"/"+name).c_str(),ofstream::binary|ios::trunc);
		}catch(...){
			error_log("Error: distfile.cpp: combine_segments(): opening distfile:"+settings.distfiles_dir+"/"+name);
			return 1;
		}
		char * buffer;
		ulong cur_seg_size;
	//  char * buffer;
	//  buffer = new char [size];
		try{
			for (uint seg_num=0; seg_num < segments_count; seg_num++){
				debug("Joining "+name+" segment "+toString(seg_num)+"          ");
				ifstream segment_file;
				segment_file.exceptions (ofstream::failbit | ofstream::badbit);
				try{
					segment_file.open((settings.segments_dir+"/"+dn_segments[seg_num].file_name).c_str(),ifstream::binary);
				}catch(...){
					error_log("Error: distfile.cpp: combine_segments(): opening segment:"+settings.segments_dir+"/"+dn_segments[seg_num].file_name);
					return 2;
				}
				try{
					// get size of file
					ulong start=segment_file.tellg();
					segment_file.seekg(0,ifstream::end);
					ulong end=segment_file.tellg();
					cur_seg_size=end-start;
					segment_file.seekg(0);
					debug(">>>>>SEG:"+dn_segments[seg_num].file_name+" Start: "+toString(start)+" End: "+toString(end)+" Size: "+toString(cur_seg_size));
					// allocate memory for file content
					buffer = new char [cur_seg_size];
					// read content of infile
					segment_file.read (buffer,cur_seg_size);
					segment_file.close();
				}catch(...){
					error_log("Error: distfile.cpp: combine_segments(): segment is open, but error occured while reading it:"+settings.segments_dir+"/"+dn_segments[seg_num].file_name);
						return 3;
				}
				try{
					// write to outfile
					distfile_file.write (buffer,cur_seg_size);
				}catch(...){
					error_log("Error: distfile.cpp: combine_segments(): distfile is open, but error occured while writing into it:"+settings.distfiles_dir+"/"+name);
					return 4;
				}
				// release dynamically-allocated memory
				delete[] buffer;
					if(remove((settings.segments_dir+"/"+dn_segments[seg_num].file_name).c_str()) != 0 )
						error_log("Tdistfile::combine_segments() - Error: can't delete:"+settings.segments_dir+"/"+dn_segments[seg_num].file_name);
					else
						debug(settings.segments_dir+"/"+dn_segments[seg_num].file_name+" deleted" );
			}
	
			distfile_file.close();
			stats.inc_dld_distfiles_count();
			log("Distfile "+name+" has been combined");
		}catch(...){
			error_log("Error in distfile.cpp: combine_segments() for distfile:"+settings.distfiles_dir+"/"+name);
			return 5;
		}
		
		try{
			if (rmd160_ok(settings.distfiles_dir+"/"+name,RMD160))
				log("RMD160 checksum for distfile:"+name+" is [OK]");
			else{
				log("Error: RMD160 checksum for distfile:"+name+" [FAILED]");
				error_log("Error: RMD160 checksum for distfile:"+name+" [FAILED]");
			}
	
			if (sha1_ok(settings.distfiles_dir+"/"+name,SHA1))
				log("SHA1   checksum for distfile:"+name+" is [OK]");
			else{
				log("Error: SHA1   checksum for distfile:"+name+" [FAILED]");
				error_log("Error: SHA1   checksum for distfile:"+name+" [FAILED]");
			}
	
			if (sha256_ok(settings.distfiles_dir+"/"+name,SHA256))
				log("SHA256 checksum for distfile:"+name+" is [OK]");
			else{
				log("Error: SHA256 checksum for distfile:"+name+" [FAILED]");
				error_log("Error: SHA256 checksum for distfile:"+name+" [FAILED]");
			}
		}catch(...){
			error_log("Error: distfile.cpp: combine_segments() for segment:"+settings.distfiles_dir+"/"+name+" while checking checksums.");
			return 6;
		}
	}catch(...){
		error_log("Error: distfile.cpp: combine_segments() for segment:"+settings.distfiles_dir+"/"+name+" during procedure.");
		return 7;
	}
	return 0;
}