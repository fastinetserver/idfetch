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

#include "segget.h"

int load_pkgs(){
	debug("uuuuuuuuuuuuuummmmmmmmmmmmmmmm");
	try{
		
		ifstream json_pkg_list_file;
		json_pkg_list_file.exceptions (ofstream::failbit | ofstream::badbit);
		try{
			json_pkg_list_file.open((settings.pkg_list_dir+"/pkg.list").c_str());
		}catch(...){
			error_log("Error: Can't open "+settings.pkg_list_dir+"/pkg.list"+" file");
			return 1;
		}
		string buff((std::istreambuf_iterator<char>(json_pkg_list_file)), std::istreambuf_iterator<char>());
		try{
			json_pkg_list_file>>buff;
		}catch(std::ifstream::failure e){
			if (!json_pkg_list_file.eof()){
				error_log("Error: Can't read pkg.list file: "+(string)e.what());
				return 1;
			}
		}catch(...){
			error_log("Unknown Error: Can't read pkg.list file");
			return 1;
		}
		try{
			json_pkg_list_file.close();
		}catch(...){
			error_log("Error: Can't close pkg.list file");
		}
		try{
			//	cout<< buff<<"\n";
			json_object *json_array_pkg_list;
			json_array_pkg_list = json_tokener_parse(buff.c_str());
			if (is_error(json_array_pkg_list))
				printf("error parsing json: %s\n",json_tokener_errors[-(unsigned long)json_array_pkg_list]);
			else {
				stats.pkg_count=json_object_array_length(json_array_pkg_list);
				Ppkg_array= new Tpkg* [stats.pkg_count];
				// create 0 pkg for distfiles to provide proxy-fetcher
				for(uint array_item_num=0;array_item_num<stats.pkg_count;array_item_num++){
					Ppkg_array[array_item_num]=new Tpkg;
					Ppkg_array[array_item_num]->load_pkg_from_json(json_object_array_get_idx(json_array_pkg_list,array_item_num));
				}
			}
			return 0;
		}catch(...){
			error_log("Error: Can't parse json data from pkg.list file");
			return 1;
		}
	}catch(...){
		error_log("Error in segget.cpp: load_pkgs()");
		return 1;
	}
}
/*
void show_pkgs(){
	try{
		for (uint array_item_num=0;array_item_num<stats.pkg_count;array_item_num++){
			cout <<"PKG:"<<array_item_num<<") cat:"<< Ppkg_array[array_item_num]->category <<" name:"<< Ppkg_array[array_item_num]->name <<"\n";
			for(uint distfile_array_item_num=0;distfile_array_item_num<Ppkg_array[array_item_num]->distfile_count;distfile_array_item_num++){
				cout << "    "<< distfile_array_item_num<<") distfile_name:"<< Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->name<<"\n";
				for(uint urllist_array_item_num=0;urllist_array_item_num<Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->url_count;urllist_array_item_num++){
					cout <<"        "<<Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->url_list[urllist_array_item_num]<<"\n";
				}
			}
		}
	}catch(...){
		error_log("Error in segget.cpp: show_pkgs()");
	}
}
*/
int pkg_choose_segment(Tpkg * cur_pkg, uint connection_num){
		uint distfile_num(0);
		uint segment_num(0);
			while(distfile_num<cur_pkg->distfile_count){
//				if (Ppkg_array[pkg_num]->distfile_vector[distfile_num].allows_new_actions()){
				if (cur_pkg->Pdistfile_list[distfile_num]->allows_new_actions()){
						debug("segment_num:"+toString(segment_num));
						debug("segment_count:"+toString(cur_pkg->Pdistfile_list[distfile_num]->segments_count));
						while (segment_num<cur_pkg->Pdistfile_list[distfile_num]->segments_count){
							debug("segment_num:"+toString(segment_num));
//							debug("		segment_num:"+toString(segment_num));
							//	segments_in_progress[connection_num]=
							//	if not(Ppkg_array[pkg_num]->Pdistfile_list[distfile_num]->get_segment_downloaded_status(segment_num);
							debug("Let's get segment status: "+statusToString(cur_pkg->Pdistfile_list[distfile_num]->dn_segments[segment_num].status));
							if (cur_pkg->Pdistfile_list[distfile_num]->dn_segments[segment_num].status==SWAITING){
								if (0==cur_pkg->Pdistfile_list[distfile_num]->provide_segment(cm, connection_num, segment_num)){
									return 0; // download started
								};
								// no success with this segment though it's waiting
								// probably there will be a problem downloading next segment
								// so let's go for the next distfile
								// and return to this distfile later - next time pkg_choose_segment is called
								break; 
							}else{
								debug("status is not SWAITING - go for the next segment");
								// haven't managed to provide this segment, go for the next one
								segment_num++;
							}
						}
				}
				distfile_num++;
				segment_num=0;
			}
			distfile_num=0;
	return 1;
}

int choose_segment(uint connection_num){
	try{
		for (uint pkg_num=0; pkg_num<stats.pkg_count; pkg_num++){
//			debug("pkg_num:"+toString(pkg_num));
			if (0==pkg_choose_segment(Ppkg_array[pkg_num], connection_num)){
				return 0;
			}
		}
		// download distfiles as a proxy-fetcher
		if (0==pkg_choose_segment(&proxy_fetcher_pkg, connection_num)){
			return 0;
		}
		//  for (uint array_item_num=0;array_item_num<pkg_count;array_item_num++){
		//for(int distfile_array_item_num=0;distfile_array_item_num<Ppkg_array[array_item_num]->distfile_count;distfile_array_item_num++){
		//  if (Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->url_count)
		//	Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->dosegments();
		//}
		//}
		return 1;
	}catch(...){
		error_log("Error: segget.cpp : choose_segment()");
		return 1;
	}
}
int download_pkgs(){
	try{
		//  cout << "Need to download: " << segments_count << " segments\n";
		CURLMsg *msg;
		long L;
		int M, Q, U = -1;
		fd_set R, W, E;
		struct timeval T;

		try{
			curl_global_init(CURL_GLOBAL_ALL);
			cm = curl_multi_init();
			// we can optionally limit the total amount of connections this multi handle uses
			curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, (long)MAX_CONNECTS);
		}catch(...){
			error_log("Error in segget.cpp: download_pkgs() : while calling curl_global_init() and curl_multi_init()");
			return EXIT_FAILURE;
		}

//		for (uint connection_num = 0; connection_num < settings.max_connections; ++connection_num) {
//			if ( ! connection_array[connection_num].active){
//				if (choose_segment(connection_num))
//					break;
//			}
//		};
		bool keep_running_flag=true;
		struct timeval prev_connection_activation_cycle_time;
		while (keep_running_flag) {
			// Use free connections to download segments connections
			if (1000>time_left_from(prev_connection_activation_cycle_time)){
				debug("Not enough time left to start connection activation cycle");
				sleep(1);
			}else{
				debug("Entering connection activation cycle");
				gettimeofday(&prev_connection_activation_cycle_time,NULL);
				for (uint connection_num = 0; connection_num < settings.max_connections; ++connection_num) {
					debug("connection_num:"+toString(connection_num));
					if ( ! connection_array[connection_num].active){
						debug("connection is not active - choosing segment");
						if (choose_segment(connection_num)){
						// if no success don't try the same for other connections
							debug("No segment found for connection:"+toString(connection_num)+" => no reason to look for a segment for other connections");
							break;
						}else{
//							U++;
						}
					}
					else{
						debug("connection is active");
					}
				};
				debug("Exit connection activation sycle");
			}
			U=stats.active_connections_counter;
			debug("before multi_perform");
//			while (CURLM_CALL_MULTI_PERFORM == curl_multi_perform(cm, &U)){};
			if (CURLM_CALL_MULTI_PERFORM != curl_multi_perform(cm, &U)){
				debug("after multi_perform");
//				if (U) {
					FD_ZERO(&R);
					FD_ZERO(&W);
					FD_ZERO(&E);
					if (curl_multi_fdset(cm, &R, &W, &E, &M)) {
						error_log("Error: curl_multi_fdset");
//						fprintf(stderr, "E: curl_multi_fdset\n");
						return EXIT_FAILURE;
					}
					if (curl_multi_timeout(cm, &L)) {
						error_log("Error: curl_multi_timeout");
//						fprintf(stderr, "E: curl_multi_timeout\n");
						return EXIT_FAILURE;
					}
					if (L == -1)
						L = 100;
					if (M == -1) {
						#ifdef WIN32
						Sleep(L);
						#else
							sleep(L / 1000);
						#endif
					} else {
						T.tv_sec = L/1000;
						T.tv_usec = (L%1000)*1000;
						if (0 > select(M+1, &R, &W, &E, &T)) {
//							fprintf(stderr, "E: select(%i,,,,%li): %i: %s\n",
							error_log("Error: select ("+toString(M+1)+","+toString(L)+"):"+toString(errno)+": "+strerror(errno));
							return EXIT_FAILURE;
						}
					}
//				}
				debug("before multi_info_read");
				while ((msg = curl_multi_info_read(cm, &Q))) {
					debug("inside multi_info_read");
					if (msg->msg == CURLMSG_DONE) {
						Tsegment *current_segment;
						CURL *e = msg->easy_handle;
						curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &current_segment);
						int connection_result=msg->data.result;
						string result_msg_text="RESULT:"+toString(connection_result)+" "+curl_easy_strerror(msg->data.result)+" while downloading segment";
						msg_status1(current_segment->connection_num,current_segment->segment_num,result_msg_text);
						curl_multi_remove_handle(cm, e);
	
						connection_array[current_segment->connection_num].stop(connection_result);
	
						if (not choose_segment(current_segment->connection_num)) {
							U++; // just to prevent it from remaining at 0 if there are more URLs to get
						};
						stats.show_totals();
						curl_easy_cleanup(e);
					}else {
						error_log("ERROR: CURLMsg: "+msg->msg);
					}
				}
			}
		}
		curl_multi_cleanup(cm);
		curl_global_cleanup();
		log("Nothing to download - Exiting... (Press any key to exit) ");
		return EXIT_SUCCESS;
	}catch(...){
		error_log("Error in segget.cpp: download_pkgs()");
		return EXIT_FAILURE;
	}
}

void launch_tui_thread(){
	pthread_t tui_thread;
	int iret1;
	iret1 = pthread_create( &tui_thread, NULL, print_message_function, (void*) NULL);
}

void launch_ui_server_thread(){
	pthread_t ui_server_thread;
	int iret1;
	debug_no_msg("Creating ui_server_thread.");
	ui_server.init();
	iret1 = pthread_create( &ui_server_thread, NULL, run_ui_server, (void*) NULL);
	debug_no_msg("ui_server_thread launched");
}

void launch_proxy_fetcher_server_thread(){
	if (settings.provide_proxy_fetcher_ip!="none"){
		pthread_t proxy_fetcher_server_thread;
		int iret1;
		debug_no_msg("Creating ui_server_thread.");
//		proxy_fetcher_server_thread.init();
		iret1 = pthread_create( &proxy_fetcher_server_thread, NULL, run_proxy_fetcher_server, (void*) NULL);
		debug_no_msg("proxy_fetcher_server_thread launched");
	}
}

void segget_exit(int sig){
	try{
		endwin();
	}
	catch(...)
	{
		//error while ending curses
	}
	try{
		for(uint fd = 0; fd <= ui_server.max_fd_num; fd++) {
			close(fd);
		}
	}
	catch(...)
	{
		//error while clossing server_sockfd
	}
	error_log_no_msg("Segget exits because cought signal:"+toString(sig));
	cout<<"Bye!\n\n";
	exit(0);
}

int main()
{
	try{
		signal(SIGABRT,segget_exit);//If program aborts go to assigned function "segget_exit".
		signal(SIGTERM,segget_exit);//If program terminates go to assigned function "segget_exit".
		signal(SIGINT,segget_exit);//If program terminates go to assigned function "segget_exit".
		prev_time=time((time_t *)NULL);
		try{
			//init curses
			initscr();
			curs_set(0);
			refresh();
		}
		catch(...)
		{
			//error while init curses
		}
		try{
			//load settings
				settings.init();
		}
		catch(...)
		{
			//error while loading settings
		}
		try{
			init_connections();
		}catch(...){
			error_log("error while init_connections");
		}
		try{
			launch_ui_server_thread();
		}catch(...){
			error_log_no_msg("Error in segget.cpp launch_ui_server() failed");
		}
		try{
			launch_proxy_fetcher_server_thread();
		}catch(...){
			error_log_no_msg("Error in segget.cpp launch_proxy_fetcher_server_thread failed");
		}
		try{
			launch_tui_thread();
		}catch(...){
			error_log_no_msg("Error in segget.cpp launch_tui_theread() failed");
		}
		try{
			load_pkgs();
		}
		catch(...){
			//error while loading pkgs
		}
		try{
			//show_pkgs();
			stats.show_totals();
		}
		catch(...){
			//error while showing stats
		}
		try{
			download_pkgs();
		}
		catch(...){
			//error while downloading_pkgs
		}
		getch();
	}
	catch(...){
		//error during init and downloading process
	}
	try{
		endwin();
	}
	catch(...)
	{
		//error while ending curses
	}
	try{
		close(ui_server.server_sockfd);
	}
	catch(...)
	{
		//error while clossing server_sockfd
	}
	return 0;
}

void *print_message_function(void * ){
	while (true){
		ulong time_diff_msecs=time_left_from(stats.previous_time);
		if (time_diff_msecs >= settings.current_speed_time_interval_msecs){
			show_progress(time_diff_msecs);
		};
		stats.show_totals();
		sleep(1);
	}
	return 0;
}