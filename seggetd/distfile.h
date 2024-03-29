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

#ifndef __DISTFILE_H__
#define __DISTFILE_H__
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#  include <unistd.h>
#endif
#include <curl/multi.h>

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <json/json.h>
#ifdef WINDOWS
	#include <direct.h>
	#define GetCurrentDir _getcwd
#else
	#include <unistd.h>
	#define GetCurrentDir getcwd
#endif
//class Tui_server;
#include "segment.h"
#include "networkbroker.h"
//#include "settings.h"
//class Tsettings;
class Tsegment;

//#include <sys/types.h>
//#include <sys/socket.h>
//#include <stdio.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <sys/ioctl.h>
//#include <unistd.h>
//#include <stdlib.h>
//#include <string>
//#include <string.h>


//#include "client.cpp"

using namespace std;

#define MAX_NETWORKS 10
typedef unsigned int uint;

enum Tdistfile_status{
	DNEW,
	D_NOT_PROXY_REQUESTED,
	DPROXY_REJECTED,
	DPROXY_QUEUED,
	DPROXY_DOWNLOADING,
	DPROXY_DOWNLOADED,
	DPROXY_FAILED,
	DALL_LM_AND_PF_MIRRORS_FAILED,
	DWAITING,
	DSCRIPTREJECTED,
	DDOWNLOADING,
	DDOWNLOADED,
	DFAILED
};

long is_symlink_restricted(string distfile_name);

class Tdistfile{
	private:
		bool choose_best_local_mirror(CURLM* cm, uint connection_num, uint network_num, uint seg_num);
		bool choose_best_mirror(CURLM* cm, uint connection_num, uint network_num, uint seg_num);
		Tdistfile_status status;
	public:
		uint dld_segments_count;
		ulong dld_bytes;
		Tnetwork_distfile_broker network_distfile_brokers_array[MAX_NETWORKS];
		string json_data;
//		bool downloaded;
		uint active_connections_num;
		string *url_list;
		uint url_num;
		uint segment_num;
		uint segments_count;
		Tsegment *dn_segments;
		string name;
		uint num;
		ulong size;
		string RMD160;
		string SHA1;
		string SHA256;
		string SHA512;
		string WHIRLPOOL;
		string MD5;
		string CRC32;
		uint url_count;
		uint segment_size;
		Tdistfile():
			status(DNEW),
			dld_segments_count(0),
			dld_bytes(0),
			json_data(""),
//			downloaded(0),
			active_connections_num(0),
			url_list(0),
			url_num(0),
			segment_num(0),
			segments_count(0),
			dn_segments(0),
			name(""),
			num(0),
			size(0),
			RMD160(""),
			SHA1(""),
			SHA256(""),
			SHA512(""),
			WHIRLPOOL(""),
			MD5(""),
			CRC32(""),
			url_count(0),
//			segment_size(settings.max_segment_size)
			segment_size(500000)
			{init();};
		Tdistfile(const Tdistfile &L);             // copy constructor
		Tdistfile & operator=(const Tdistfile &L);
		~Tdistfile();
		string statusToString();
		string statusToColor();
		int request(ulong network_num, string msg);
		void init();
		void set_status(Tdistfile_status new_status);
		Tdistfile_status get_status(){return status;};
		bool allows_new_actions();
		bool load_distfile_from_json(json_object* json_obj_distfile);
		void load_url_list(json_object* json_array_distfile_urllist);
		void split_into_segments();
		uint provide_local_network(CURLM* cm, uint connection_num, uint seg_num, uint network_priority);
		uint request_proxy_fetcher_network(uint network_priority);
		uint provide_remote_or_coral_cdn_network(CURLM* cm, uint connection_num, uint seg_num, uint network_priority);
		int provide_segment(CURLM* cm, uint connection_num, uint seg_num);
		string get_distfile_progress_str();
		void inc_dld_segments_count(Tsegment * current_segment);
		void symlink_distfile_to_provide_mirror_dir();
		int combine_segments();
		bool check_if_dld();
};

#endif