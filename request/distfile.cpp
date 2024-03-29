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

//Make the necessary includes and set up the variables:
using namespace std;

/*
#define R_LM_WAIT_FOR_LOCAL_MIRRORS						100

#define R_PF_ADDED_TO_PROXY_QUEUE						101
#define R_PF_ALREADY_WAS_IN_QUEUE						102
#define R_PF_DOWNLOADED									103
#define R_PF_BE_MORE_PATIENT							104
#define R_PF_ERROR_ADDING_TO_PROXY_QUEUE				105
#define R_PF_FAILED										106
#define R_PF_REJECTED									107

// 0 for succesfull return of provide_segment()
#define R_R_DOWNLOAD_STARTED							0
#define R_R_WAITING										108
#define R_R_DOWNLOADING									109

#define R_LM_PF_R_NO_FREE_NETWORK_CONNECTION_FOUND		110

#define ALLOW_PROXY_FETCHER_NETWORKS					201
#define DO_NOT_ALLOW_PROXY_FETCHER_NETWORKS				202
#define ALLOW_REMOTE_NETWORKS							203
#define DO_NOT_ALLOW_REMOTE_NETWORKS					204
#define ALLOW_LOWER_PRIORITY_NETWORKS					205
*/


bool Tdistfile::allows_new_actions(){
//	if (downloaded) return false;
//	else return true;
//	int time_left=0;
	if (status==DDOWNLOADED){
		debug("No new connection for distfile:"+name+". Distfile has DDOWNLOADED status");
		return false;
	}
	if (status==DFAILED){
		debug("No new connection for distfile:"+name+". Distfile has DFAILED status");
		return false;
	}
//	debug("Distfile "+Ppkg_array[pkg_num]->distfile_vector[distfile_num]->name+" allows new connections");
//	debug("	distfile_num:"+toString(distfile_num));
	if (active_connections_num<settings.max_connection_num_per_distfile){
		debug("Allow new connection for ============================================= Distfile "+name);
		debug("max_connection limit has not been reached");
		return true;
	}else{
		debug("No new connection for distfile: "+name+". It already has "
			+toString(active_connections_num)
			+" connections => choose another distfile.");
			return false;
	}
//	if (((status==DPROXY_QUEUED) || (status==DPROXY_DOWNLOADING)) && (time_left<100)) return false;
//oterwise allow connections
//	DNEW,
//	D_NOT_PROXY_REQUESTED,
//	DPROXY_REJECTED,
//	DPROXY_FAILED,
//	DPROXY_DOWNLOADED,
//	DWAITING,
//	DDOWNLOADING,
}

void Tdistfile::init(){
	try{
		for (uint network_num=0; network_num<MAX_NETWORKS; network_num++){
			network_distfile_brokers_array[network_num].init(network_num);
		}
	}catch(...){
		error_log("Error: distfile.cpp: init()");
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
			status=DDOWNLOADED;
//			downloaded=true;
			num=++stats.distfiles_count;
			stats.inc_total_size(size);
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
			json_data=json_object_to_json_string(json_obj_distfile);
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
		if (dn_segments[segment_num].status==SDOWNLOADED)
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
	try{
		Tmirror *Pcurr_mirror;
		Tmirror *Pbest_mirror=0;	// the best isn't set let's find it
		uint best_mirror_num=-1;	// the best isn't set let's find it

		ulong best_mirror_self_rating=-1;
		ulong curr_mirror_self_rating;

		for (url_num=0; url_num<url_count; url_num++){
			Pcurr_mirror=find_mirror(strip_mirror_name(url_list[url_num]));
			debug("Evaluating url:"+url_list[url_num]
					+" active_connections:"+toString(Pcurr_mirror->get_active_num())
					+" connection limit:"+toString(settings.max_connections_num_per_mirror));
			if (Pcurr_mirror->get_active_num()<settings.max_connections_num_per_mirror){
				debug("Url satisfies connection num per mirror limit");
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
//			Pbest_mirror->start();
//			active_connections_num++;
			connection_array[connection_num].start(cm, network_num, num, &dn_segments[seg_num], best_mirror_num);
			return R_R_DOWNLOAD_STARTED;
		}
		else{
			error_log("Can't choose mirror for segment:"+dn_segments[seg_num].file_name);
			return 1;
		}
	}catch(...){
		error_log("Error: distfile.cpp: choose_best_mirror()");
		return 1;
	}
}
bool Tdistfile::choose_best_local_mirror(CURLM* cm, uint connection_num, uint network_num, uint seg_num){
	try{
		long best_mirror_num=-1;	// the best isn't set let's find it
		bool all_mirrors_failed=true;
		ulong best_mirror_self_rating=-1;
		ulong curr_mirror_self_rating;
		debug("Choosing mirror for network"+toString(network_num));
		for (ulong mirror_num=0; mirror_num<network_array[network_num].benchmarked_mirror_list.size(); mirror_num++){
			debug("Evaluating mirror:"+network_array[network_num].benchmarked_mirror_list[mirror_num].url);
			if (network_distfile_brokers_array[network_num].mirror_fails_vector[mirror_num]){
				debug("mirror:"+network_array[network_num].benchmarked_mirror_list[mirror_num].url+" has FAILED status - will be skipped");
			}else{
				all_mirrors_failed=false;
				if (network_array[network_num].benchmarked_mirror_list[mirror_num].get_active_num()<settings.max_connections_num_per_mirror){
					curr_mirror_self_rating=network_array[network_num].benchmarked_mirror_list[mirror_num].mirror_on_the_wall();
					if (curr_mirror_self_rating<best_mirror_self_rating){
						best_mirror_num=mirror_num;
						best_mirror_self_rating=curr_mirror_self_rating;
					}
					if (best_mirror_self_rating==0)
						// 0 can not be improved - it's one of the best
						break;
				}else{
					debug("Mirror already has:"
						+toString(network_array[network_num].benchmarked_mirror_list[mirror_num].get_active_num())
						+" connections which doesn't meet limit:"+toString(settings.max_connections_num_per_mirror));
				}
			}
		}
		if (best_mirror_num!=-1){
			debug("Downloading from BEST_LOCAL_MIRROR:"+network_array[network_num].benchmarked_mirror_list[best_mirror_num].url);
//			active_connections_num++;
			connection_array[connection_num].start(cm, network_num, num, &dn_segments[seg_num], best_mirror_num);
			return R_R_DOWNLOAD_STARTED;
		}
		else{
			debug("Can't choose LOCAL mirror for segment:"+dn_segments[seg_num].file_name);
			error_log("Can't choose LOCAL mirror for segment:"+dn_segments[seg_num].file_name);
			if (all_mirrors_failed){
				debug("All local mirrors failed in network#"+toString(network_num));
				error_log("All local mirrors failed in network#"+toString(network_num));
			}
			return 1;
		}
	}catch(...){
		error_log("Error: distfile.cpp: choose_best_local_mirror()");
		return 1;
	}
}

uint Tdistfile::provide_local_network(CURLM* cm, uint connection_num, uint seg_num, uint network_priority){
	try{
		debug("network_priority="+toString(network_priority));
		//choose network
//----------------------------------------------------------------------------------------------------------
//
//       Several criterions can be used here to choose among networks with equal priority:
// min_active_connection, min_ratio_active_to_max_connection, best_speed_accocding_to_stats, etc
// add these options to segget.conf file
//
//----------------------------------------------------------------------------------------------------------
		int best_local_network_num=-1;
		bool allow_proxy_fetcher_mirrors=true;
		for (uint network_num=0; network_num<MAX_NETWORKS; network_num++){
			if (network_array[network_num].priority==network_priority){
				debug("        network_priority="+toString(network_array[network_num].priority));
				if (network_array[network_num].network_mode==MODE_LOCAL){
					debug("             network#:"+toString(network_num)+" has MODE_LOCAL");
					debug("             phase#"+phaseToString(network_distfile_brokers_array[network_num].phase));
					if (network_distfile_brokers_array[network_num].phase==E_USE_AS_LOCAL_MIRRORS){
						if (network_array[network_num].has_free_connections()){
							debug("             has free connections");
//						debug("             Allowed network#:"+toString(network_num));
							if ((best_local_network_num==-1)
							or (network_array[best_local_network_num].active_connections_num>network_array[network_num].active_connections_num)){
									best_local_network_num=network_num;
									debug("             Replace best LOCAL network to network#:"+toString(network_num));
								}
						}else{
							debug("             does NOT have free connections");
							if (network_array[network_num].only_local_when_possible){
								allow_proxy_fetcher_mirrors=false;
								debug("Network"+toString(network_num)+" forbids using remote mirrors because not all local mirrors have failed");
							}
						}
					}
				}
				if (network_array[network_num].network_mode==MODE_PROXY_FETCHER){
					debug("             network#:"+toString(network_num)+" has MODE_PROXY_FETCHER");
					debug("             with phase:"+phaseToString(network_distfile_brokers_array[network_num].phase));
					if ((network_distfile_brokers_array[network_num].phase==E_USE_AS_LOCAL_MIRRORS)
					or (network_distfile_brokers_array[network_num].phase==E_PROXY_FETCHER_DOWNLOADED))
					{
						debug("phase fits");
						if (network_array[network_num].has_free_connections()){
							debug("           has free connections");
//							debug("             Allowed network#:"+toString(network_num));
							if ((best_local_network_num==-1)
							or (network_array[best_local_network_num].active_connections_num>network_array[network_num].active_connections_num)){
									best_local_network_num=network_num;
									debug("             Replace best LOCAL network to network#:"+toString(network_num));
							}
						}else{
							if (network_array[network_num].only_local_when_possible){
									allow_proxy_fetcher_mirrors=false;
									debug("Network"+toString(network_num)+" forbids using remote mirrors because not all local mirrors have failed");
							}
						}
					}
				}
			}
		}
		if (best_local_network_num!=-1){
			//best network has been found
								//work with network
			debug("             So best LOCAL network is network#:"+toString(best_local_network_num));
			return choose_best_local_mirror(cm, connection_num, best_local_network_num, seg_num);
		}else{
			if (allow_proxy_fetcher_mirrors){
				return ALLOW_REQUESTS_TO_PROXY_FETCHERS;
			}else{
				debug("NOT all local mirrors have failed - restricted to local mirrors only.");
				return DO_NOT_ALLOW_REQUESTS_TO_PROXY_FETCHERS;
			}
		}
	}catch(...){
		error_log("Error: distfile.cpp: provlocal_networkwork()");
		return 1;
	}
}

uint Tdistfile::request_proxy_fetcher_network(uint network_priority){
// TO-DO: Add option to go for remote networks or low priority networks even if already DPROXY_QUEUED
// TO-DO: There can be several proxy-fetchers of the same priority level, define a rule to make a choice
	try{
		debug("network_priority="+toString(network_priority));
		//choose network
		int best_proxy_fetcher_network_num=-1;
		bool allow_remote_mirrors=true;
		for (uint network_num=0; network_num<MAX_NETWORKS; network_num++){
			if (network_array[network_num].priority==network_priority){
				if (network_array[network_num].network_mode==MODE_PROXY_FETCHER){
					//replace this one by does_not_reject_connections
//					if (network_array[network_num].has_free_connections()){
					if ((network_distfile_brokers_array[network_num].phase==E_USE_AS_LOCAL_MIRRORS)
					or (network_distfile_brokers_array[network_num].phase==E_ALL_LOCAL_MIRRORS_FAILED)){
						debug("------//////////// Good phase !!!!!!!!!!!!!!!");
						decode_server_response(toString(network_distfile_brokers_array[network_num].proxy_fetcher_response));
						switch (network_distfile_brokers_array[network_num].proxy_fetcher_response){
							case R_PF_DOWNLOADED:break;
							case R_PF_ERROR_ADDING_TO_PROXY_QUEUE:break;
							case R_PF_FAILED:break;
							case R_PF_REJECTED:break;
							default:{
								//if less then 30 secs left don't bother proxy-fetcher
								if (2000>time_left_from(network_distfile_brokers_array[network_num].last_request_time)){
									debug("====================== Be more patient with proxy-fetcher - network#:"+toString(network_num)
											+" because time left:"+toString(time_left_from(network_distfile_brokers_array[network_num].last_request_time)));
									return R_PF_BE_MORE_PATIENT;
								}else{
//						(network_array[best_proxy_fetcher_network_num].active_connections_num>network_array[network_num].active_connections_num)
									best_proxy_fetcher_network_num=network_num;
									debug("-------------------------------");
									debug("             Replace best_proxy_fetcher network_num to network#:"+toString(best_proxy_fetcher_network_num));
								};
								break;
							}
						}
					}
					if (best_proxy_fetcher_network_num!=-1) break;
/*
					}else{
						if (network_array[network_num].only_local_when_possible){
							if (network_distfile_brokers_array[network_num].some_mirrors_have_NOT_failed_yet()){
								allow_remote_mirrors=false;
								debug("Network"+toString(network_num)+" forbids using remote mirrors because not all local mirrors have failed");
							}
						}
					}
*/
				}
			}
		}
		if (best_proxy_fetcher_network_num != -1){
				// request from proxy fethcer
				network_distfile_brokers_array[best_proxy_fetcher_network_num].proxy_fetcher_response=request(best_proxy_fetcher_network_num, json_data);
				if ((network_distfile_brokers_array[best_proxy_fetcher_network_num].proxy_fetcher_response==R_PF_DOWNLOADED)
				and (network_distfile_brokers_array[best_proxy_fetcher_network_num].phase==E_ALL_LOCAL_MIRRORS_FAILED))
				{
					network_distfile_brokers_array[best_proxy_fetcher_network_num].phase=E_PROXY_FETCHER_DOWNLOADED;
				}
				debug("Trying to dowload distfile"+name+" via proxy_fetcher. status="+toString(status));
				if (network_distfile_brokers_array[best_proxy_fetcher_network_num].phase==E_PROXY_FETCHER_DOWNLOADED){
					// start download from the proxy_fetcher
					debug("             So best proxy_fetcher_network is network#:"+toString(best_proxy_fetcher_network_num));
					return R_PF_DOWNLOADED;
						//choose_best_local_mirror(cm, connection_num, best_proxy_fetcher_network_num, seg_num);
				}
				//return - don't switch to low priority networks
				return network_distfile_brokers_array[best_proxy_fetcher_network_num].proxy_fetcher_response;
		}else{
			if (allow_remote_mirrors){
				debug("             So best_proxy_fetcher_failed num="+toString(best_proxy_fetcher_network_num));
				return ALLOW_REMOTE_NETWORKS;
			}else{
				debug("NOT all proxy-fetcher mirrors have failed - restricted to proxy-fetcher mirrors only.");
				return DO_NOT_ALLOW_REMOTE_NETWORKS;
			}
		}
	}catch(...){
		error_log("Error: distfile.cpp: provproxy_fetcher_networkwork()");
		return 1;
	}
}

uint Tdistfile::provide_remote_network(CURLM* cm, uint connection_num, uint seg_num, uint network_priority){
	try{
		debug("network_priority="+toString(network_priority));
		//choose network
//----------------------------------------------------------------------------------------------------------
//
//       Several criterions can be used here to choose among networks with equal priority:
// min_active_connection, min_ratio_active_to_max_connection, best_speed_accocding_to_stats, etc
// add these options to segget.conf file
//
//----------------------------------------------------------------------------------------------------------
		int best_remote_network_num=-1;
		for (uint network_num=0; network_num<MAX_NETWORKS; network_num++){
			if (network_array[network_num].priority==network_priority){
				if (network_array[network_num].network_mode==MODE_REMOTE){
					if (network_array[network_num].has_free_connections()){
						if 
						((best_remote_network_num==-1)
							or
						(network_array[best_remote_network_num].active_connections_num>network_array[network_num].active_connections_num)){
							best_remote_network_num=network_num;
							debug("             Replace best_remote_network_num to network#:"+toString(network_num));
						}
					}
				}
			}
		}
		if (best_remote_network_num!=-1){
		//best network has been found
						//work with network
			debug("             So best network is network#:"+toString(best_remote_network_num));
			return choose_best_mirror(cm, connection_num, best_remote_network_num, seg_num);
		}
		// haven't found anything suitable
		return ALLOW_LOWER_PRIORITY_NETWORKS;
	}catch(...){
		error_log("Error: distfile.cpp: provide_remote_network()");
		return 1;
	}
}

int Tdistfile::provide_segment(CURLM* cm, uint connection_num, uint seg_num){
	try{
		int result=R_NOT_SET;
		for (uint network_priority=10; network_priority>0; network_priority--){
			do {
				debug("Switching to local networks with priority:"+toString(network_priority));
				result=provide_local_network(cm, connection_num, seg_num, network_priority);
				if (result==ALLOW_REQUESTS_TO_PROXY_FETCHERS){
					debug("Requesting proxy-fetcher networks with priority:"+toString(network_priority));
					result=request_proxy_fetcher_network(network_priority);
					switch (result){
						case ALLOW_REMOTE_NETWORKS:{
							debug("Switching to remote networks with priority:"+toString(network_priority));
							result=provide_remote_network(cm, connection_num, seg_num, network_priority);
							// if not lower_priority_networks => result found => return it
							if (result!=ALLOW_LOWER_PRIORITY_NETWORKS){
								return result;
							};
							break;
						}
						// don't switch to remote and lower priority networks => return results
						case DO_NOT_ALLOW_REMOTE_NETWORKS: return result;
						case R_PF_DOWNLOADED: {debug("Yep, R_PF_DONWLOADED => LOOP AGAIN"); break;};
						// if already in queue, etc...
						// don't switch to remote and lower priority networks => return results
						default: return result;
					}
				}else{
					// don't switch to proxy-fetcher requests, remote,
					// and lower priority networks => return results
					return result;
				}
			// try to download from local mirrors again because one of the proxy-fetcher
			// claims that distfile has been donwloaded
			debug("----- JUST BEFORE THE LOOP ----");
			}while (result==R_PF_DOWNLOADED);
		}
		// haven't found anything suitable
		return R_LM_PF_R_NO_FREE_NETWORK_CONNECTION_FOUND;
	}catch(...){
		error_log("Error: distfile.cpp: provide_segment()");
		return 1;
	}
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
long is_symlink_restricted(string distfile_name){
	for(ulong pattern_num=0; pattern_num<settings.provide_mirror_files_restricted_patterns_vector.size(); pattern_num++){
		if (distfile_name.find(settings.provide_mirror_files_restricted_patterns_vector[pattern_num],0)!=distfile_name.npos){
			return pattern_num;
		}
	}
	return -1;
}

void Tdistfile::symlink_distfile_to_provide_mirror_dir(){
	try{
		long pattern_num=is_symlink_restricted(name);
		if (pattern_num>=0){
			log("Symlink to distfile:"+name+" was restricted by pattern <"
					+settings.provide_mirror_files_restricted_patterns_vector[pattern_num]
					+"> from line "+toString(pattern_num+1)+" of restrict.conf file");
			return;
		}
		string new_mirror_name;
		string old_distfile_name;
		try{
			string old_distfile_path;
			char current_path[FILENAME_MAX];
			if (!GetCurrentDir(current_path, sizeof(current_path)))
			{
				return;
			}
			if (settings.distfiles_dir.find("./",0)==0){
				old_distfile_path=current_path+settings.distfiles_dir.substr(1,settings.distfiles_dir.npos);
			}else{
				old_distfile_path=settings.distfiles_dir;
			}
			new_mirror_name=settings.provide_mirror_dir+"/"+name;
			old_distfile_name=old_distfile_path+"/"+name;
			try{
				if (!symlink(old_distfile_name.c_str(), new_mirror_name.c_str())){
					log("Distfile:"+old_distfile_path+" was symlinked to the mirror dir:");
				};
			}catch(uint errno){
				switch (errno){
					case EACCES			: error_log("Write access to the directory containing "+settings.provide_mirror_dir+" is denied, or one of the directories in the path prefix of "+settings.provide_mirror_dir+" did not allow search permission.  (See also path_resolution(7).");
											break;
					case EEXIST			: error_log("There is already an existing file named "+new_mirror_name+".");
											break;
					case EFAULT			: error_log(old_distfile_path+" or "+settings.provide_mirror_dir+" points outside your accessible address space.");
											break;
					case EIO			: error_log("A hardware error occurred while reading or writing data on the disk.");
											break;
					case ELOOP			: error_log("Too many symbolic links were encountered in resolving "+settings.provide_mirror_dir+".");
											break;
					case ENAMETOOLONG	: error_log(old_distfile_path+" or "+settings.provide_mirror_dir+" was too long.");
											break;
					case ENOENT			: error_log("A directory component in "+settings.provide_mirror_dir+" does not exist or is a dangling symbolic link, or "+old_distfile_path+" is the empty string.");
											break;
					case ENOMEM			: error_log("Insufficient kernel memory was available.");
											break;
					case ENOSPC			: error_log("The device containing the file has no room for the new directory entry.");
											break;
					case ENOTDIR		: error_log("A component used as a directory in "+settings.provide_mirror_dir+" is not, in fact, a directory.");
											break;
					case EPERM			: error_log("The file system containing "+settings.provide_mirror_dir+" does not support the creation of symbolic links.");
											break;
					case EROFS			: error_log("The file "+new_mirror_name+" would exist on a read-only file system.");
											break;
				default:
					error_log("Undocumented error while trying to symlink "+old_distfile_name+" to "+new_mirror_name);
				}
			}catch(...){
				error_log("Undocumented error (error description is not an integer) while trying to symlink "+old_distfile_name+" to "+new_mirror_name);
			}
		}catch(...){
				error_log("Error in distfile.cpp :: symlink_distfile_to_provide_mirror_dir() while trying to symlink "+old_distfile_name+" to "+new_mirror_name);
		}
	}catch(...){
		error_log("Error: distfile.cpp: symlink_distfile_to_provide_mirror_dir()");
	}
}
int Tdistfile::combine_segments(){
	try{
		debug("Combining distfile"+name);
		ofstream distfile_file;
		distfile_file.exceptions (ofstream::failbit | ofstream::badbit);
		string distfile_path=settings.distfiles_dir+"/"+name;
		try{
			distfile_file.open(distfile_path.c_str(),ofstream::binary|ios::trunc);
		}catch(...){
			error_log("Error: distfile.cpp: combine_segments(): opening distfile:"+distfile_path);
			status=DFAILED;
			return 1;
		}
		char * buffer;
		ulong cur_seg_size;
		try{
			for (uint seg_num=0; seg_num < segments_count; seg_num++){
				debug("Joining "+name+" segment "+toString(seg_num)+"          ");
				ifstream segment_file;
				segment_file.exceptions (ofstream::failbit | ofstream::badbit);
				try{
					segment_file.open((settings.segments_dir+"/"+dn_segments[seg_num].file_name).c_str(),ifstream::binary);
				}catch(...){
					error_log("Error: distfile.cpp: combine_segments(): opening segment:"+settings.segments_dir+"/"+dn_segments[seg_num].file_name);
					status=DFAILED;
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
					status=DFAILED;
					return 3;
				}
				try{
					// write to outfile
					distfile_file.write (buffer,cur_seg_size);
				}catch(...){
					error_log("Error: distfile.cpp: combine_segments(): distfile is open, but error occured while writing into it:"+settings.distfiles_dir+"/"+name);
					status=DFAILED;
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
			status=DFAILED;
			return 5;
		}
		try{
			if (rmd160_ok(settings.distfiles_dir+"/"+name,RMD160))
				log("RMD160 checksum for distfile:"+name+" is [OK]");
			else{
				log("Error: RMD160 checksum for distfile:"+name+" [FAILED]");
				error_log("Error: RMD160 checksum for distfile:"+name+" [FAILED]");
				status=DFAILED;
				return 10;
			}
	
			if (sha1_ok(settings.distfiles_dir+"/"+name,SHA1))
				log("SHA1   checksum for distfile:"+name+" is [OK]");
			else{
				log("Error: SHA1   checksum for distfile:"+name+" [FAILED]");
				error_log("Error: SHA1   checksum for distfile:"+name+" [FAILED]");
				status=DFAILED;
				return 11;
			}
	
			if (sha256_ok(settings.distfiles_dir+"/"+name,SHA256))
				log("SHA256 checksum for distfile:"+name+" is [OK]");
			else{
				log("Error: SHA256 checksum for distfile:"+name+" [FAILED]");
				error_log("Error: SHA256 checksum for distfile:"+name+" [FAILED]");
				status=DFAILED;
				return 12;
			}
			status=DDOWNLOADED;
			if (settings.provide_mirror_dir!="none"){
				symlink_distfile_to_provide_mirror_dir();
			}
		}catch(...){
			error_log("Error: distfile.cpp: combine_segments() for segment:"+settings.distfiles_dir+"/"+name+" while checking checksums.");
			return 30;
		}
	}catch(...){
		error_log("Error: distfile.cpp: combine_segments() for segment:"+settings.distfiles_dir+"/"+name+" during procedure.");
		return 31;
	}
	return 0;
}