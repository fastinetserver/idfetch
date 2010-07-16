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

#include "settings.h"

Tsettings settings;

void Tsettings::load_provide_mirror_files_restricted_patterns_vector(){
	try{
		ifstream file;
		file.exceptions (ifstream::failbit | ifstream::badbit);
		try{
			file.open("restrict.conf");
		}
		catch(...){
			error_log("Can NOT open pattern list file restrict.conf. Setting provide_mirror_files_restrict_list_on=0 will be forced.");
			provide_mirror_files_restrict_list_on=0;
			return;
		}
		try{
			//processing file
			string restricted_pattern_line;
			while (not(file.eof())) {
				getline(file,restricted_pattern_line);
				if (! restricted_pattern_line.length()) continue;
				if (restricted_pattern_line[0] == '#') continue;
				if (restricted_pattern_line[0] == ';') continue;
				provide_mirror_files_restricted_patterns_vector.push_back(restricted_pattern_line);
				debug("restricted_pattern_line added:"+restricted_pattern_line);
			}
			log(toString(provide_mirror_files_restricted_patterns_vector.size())+" pattern(s) was(were) read from restrict.conf");
			if (! provide_mirror_files_restricted_patterns_vector.size()){
				error_log("No patterns were read from restrict.conf file. Setting provide_mirror_files_restrict_list_on=0 will be forced.");
				provide_mirror_files_restrict_list_on=0;
			}
		}
		catch(...){
			error_log("Restricted pattern list file restrict.conf was opened, but an error occured while reading it.");
		}
	}catch(...){
		error_log("Error in settings.cpp: load_provide_mirror_files_restricted_patterns_vector()");
	}
}

void Tsettings::init(){
	try{
		Tconfig conf("segget.conf");
		conf.set(general_log_file,					"logs",				"general_log_file");
		conf.set(logs_dir,							"logs",				"logs_dir");
		conf.set(error_log_file,					"logs",				"error_log_file");
		conf.set(debug_log_file,					"logs",				"debug_log_file");
		conf.set(max_connections,					"connections",		"max_connections",1,MAX_CONNECTS);

		conf.set(distfiles_dir,						"folders",			"distfiles_dir");
		conf.set(segments_dir,						"folders",			"segments_dir");

		conf.set(pkg_list_dir,						"pkg_list",			"pkg_list_dir");
		conf.set(del_pkg_list_when_dld_finished,	"pkg_list",			"del_pkg_list_when_dld_finished");

		conf.set(max_connection_num_per_distfile,	"distfiles",		"max_connection_num_per_distfile",1,MAX_CONNECTS);

		conf.set(resume_on,							"segments",			"resume_on");
		conf.set(max_segment_size,					"segments",			"max_segment_size",10000,10000000);
		conf.set(max_tries,							"segments",			"max_tries",1,-1);

		conf.set(current_speed_time_interval_msecs,	"connections",		"current_speed_time_interval_msecs",100,60000);

		conf.set(max_connections_num_per_mirror,	"mirrors",			"max_connections_num_per_mirror",1,10);
		conf.set(benchmark_oblivion,				"mirrors",			"benchmark_oblivion",0,1000);

		conf.set(provide_mirror_dir,						"provide_mirror_to_others",		"provide_mirror_dir");
		if (provide_mirror_dir!="none"){
			conf.set(provide_mirror_files_restrict_list_on,		"provide_mirror_to_others",		"provide_mirror_files_restrict_list_on");
			if (provide_mirror_files_restrict_list_on)
				load_provide_mirror_files_restricted_patterns_vector();
		}

		conf.set(provide_proxy_fetcher_ip,						"provide_proxy_fetcher_to_others",		"provide_proxy_fetcher_ip");
		conf.set(provide_proxy_fetcher_port,					"provide_proxy_fetcher_to_others",		"provide_proxy_fetcher_port",1,65535);

		ulong cur_network_priority;
		for (uint network_num=0; network_num<MAX_NETWORKS; network_num++){
			//set default values, in case segget.conf doesn't have these settings
			if (network_num==0){
				cur_network_priority=10;
			}else{
				cur_network_priority=0;
			}

			conf.set(cur_network_priority,				"networks",			"network"+toString(network_num)+"_priority",0,10);
			if (cur_network_priority>0){
				network_array[network_num].init(cur_network_priority);
			}
		}

		conf.set(ui_ip,									"ui_server",			"ui_ip");
		conf.set(ui_port,								"ui_server",			"ui_port",1,65535);

		conf.clear();
	}catch(...){
		error_log_no_msg("Error in settings.cpp: init()");
	}
}