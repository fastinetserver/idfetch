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
#include "network.h"

Tnetwork network_array[MAX_NETWORKS];

void Tnetwork::load_mirror_list(){
	try{
		ifstream file;
		string mirror_list_file_name="network"+toString(network_num)+"_mirrors.conf";
		file.exceptions (ifstream::failbit | ifstream::badbit);
		try{
			file.open(mirror_list_file_name.c_str());
		}
		catch(...){
			error_log("Can NOT open mirror list file: "+mirror_list_file_name+". Network will be disabled");
			priority=0;
			return;
		}
		try{
			//processing file
			string mirror_line;
			while (not(file.eof())) {
				getline(file,mirror_line);
				if (! mirror_line.length()) continue;
				if (mirror_line[0] == '#') continue;
				if (mirror_line[0] == ';') continue;
				Tmirror cur_mirror;
				cur_mirror.url=mirror_line;
				benchmarked_mirror_list.push_back(cur_mirror);
				debug("LOCAL_MIRROR_ADDED:"+mirror_line);
			}
		}catch(ifstream::failure e){
			if (!file.eof()){
				error_log("Mirror list file: "+mirror_list_file_name+" was opened, but an error occured while reading from it.");
				return;
			}
		}catch(...){
			error_log("Mirror list file: "+mirror_list_file_name+" was opened, but an error occured while reading from it.");
		}
	}catch(...){
		error_log_no_msg("Error in network.cpp: load_mirror_list()");
	}
}

void Tnetwork::init(uint priority_value){
	try{
		priority=priority_value;
		Tconfig conf("network"+toString(network_num)+".conf");
		conf.set("mode","network_mode",network_mode,0,3);
		conf.set("network_bind","bind_interface",bind_interface);
		conf.set("network_bind","bind_local_port",bind_local_port,0,65535);
		conf.set("network_bind","bind_local_port_range",bind_local_port_range,1,65535);
		conf.set("network_connections","max_connections",max_connections,1,MAX_CONNECTS);
		conf.set("network_connections","connection_timeout",connection_timeout,1,1000);
		conf.set("network_connections","ftp_response_timeout",ftp_response_timeout,1,-1);
		conf.set("network_connections","timeout",time_out,100,-1);
		conf.set("network_connections","low_connection_speed_limit",low_connection_speed_limit,1,-1);
		conf.set("network_connections","low_connection_speed_time",low_connection_speed_time,1,600);
		conf.set("network_connections","max_connection_speed",max_connection_speed,1,-1);
		conf.set("network_connections","follow_location",follow_location);
		conf.set("network_connections","max_redirs",max_redirs,0,100);
		conf.set("network_user_data","user_agent",user_agent);

		conf.set("network_proxy","proxy_ip_or_name",proxy_ip_or_name);
		conf.set("network_proxy","proxy_port",proxy_port,1,65535);
		conf.set("network_proxy","proxy_off",proxy_off);
		conf.set("network_proxy","proxy_user",proxy_user);
		conf.set("network_proxy","proxy_password",proxy_password);

		ulong raw_proxy_type=0;
		conf.set("network_proxy","proxy_type",raw_proxy_type,0,5);
		switch (raw_proxy_type){
			case HTTP: proxy_type=CURLPROXY_HTTP; break;
			case HTTP_1_0: proxy_type=CURLPROXY_HTTP_1_0; break;
			case SOCKS4: proxy_type=CURLPROXY_SOCKS4; break;
			case SOCKS4a: proxy_type=CURLPROXY_SOCKS4A; break;
			case SOCKS5: proxy_type=CURLPROXY_SOCKS5; break;
			case SOCKS5_HOSTNAME: proxy_type=CURLPROXY_SOCKS5_HOSTNAME; break;
			default : proxy_type=CURLPROXY_HTTP; break;
		}

//		conf.set(use_own_mirror_list_only_on,		"network_mirrors",			"use_own_mirror_list_only_on");

		switch (network_mode){
			case MODE_LOCAL:
				{
					conf.set("network_mirrors","only_local_when_possible",only_local_when_possible);
					load_mirror_list();
					log("Settings in file:network"+toString(network_num)+"_mirrors.conf local mirror_list size:"+toString(benchmarked_mirror_list.size()));
					break;
				};
			case MODE_PROXY_FETCHER:
				{
					conf.set("network_proxy_fetcher","proxy_fetcher_ip",proxy_fetcher_ip);
					if (proxy_fetcher_ip=="none"){
						error_log("Network"+toString(network_num)+" in PROXY_FETCHER mode, but proxy_fetcher_ip="+proxy_fetcher_ip+". Network will be disabled.");
						priority=0;
					}else{
						conf.set("network_proxy_fetcher","proxy_fetcher_port",proxy_fetcher_port,1,65535);
						if (proxy_fetcher_ip==settings.provide_proxy_fetcher_ip && proxy_fetcher_port==settings.provide_proxy_fetcher_port){
							error_log("Error: Network"+toString(network_num)+" settings have the same ip and port for proxy-fetcher as the ones in [provide_proxy_fetcher_to_others] section of segget.conf file.");
							error_log("       Segget instance can NOT serve as a proxy-fetcher for itself - network will be disabled");
							priority=0;
						}else{
							conf.set("network_mirrors","only_local_when_possible",only_local_when_possible);
							load_mirror_list();
							log("Settings: Network"+toString(network_num)+" local fetcher_local_mirrors_list size:"+toString(mirror_list.size()));
						}
					}
					break;
				}
			default: break; // network in MODE_REMOTE
		}
		conf.clear();
	}catch(...){
		error_log("Error in network.cpp: init()");
	}
}

bool Tnetwork::has_free_connections(){
	try{
		if (active_connections_num<max_connections){
			return true;
		}else{
			return false;
		}
	}catch(...){
		error_log_no_msg("Error in network.cpp: has_free_connections()");
		return false;
	}
}

bool Tnetwork::connect(){
	try{
		if (active_connections_num<max_connections){
			active_connections_num++;
			return true;
		}else{
			return false;
		}
	}catch(...){
		error_log_no_msg("Error in network.cpp: connect()");
		return false;
	}
}