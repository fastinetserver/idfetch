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

#include "seggetd.h"
int pkg_choose_segment(Tpkg * cur_pkg, uint connection_num){
	try{
		uint distfile_num(0);
		uint segment_num(0);
		debug("Entered pkg_choose_segment()");
		while(distfile_num<cur_pkg->distfile_count){
//			if (Ppkg_array[pkg_num]->distfile_vector[distfile_num].allows_new_actions()){
			debug("considering distfile:"+toString(distfile_num));
			if (cur_pkg->Pdistfile_list[distfile_num]->allows_new_actions()){
				debug("segment_num:"+toString(segment_num));
				debug("segment_count:"+toString(cur_pkg->Pdistfile_list[distfile_num]->segments_count));
				while (segment_num<cur_pkg->Pdistfile_list[distfile_num]->segments_count){
					debug("segment_num:"+toString(segment_num));
//					debug("		segment_num:"+toString(segment_num));
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
	}catch(...){
		error_log("Error: segget.cpp : pkg_choose_segment()");
		return 1;
	}
}

int choose_segment(uint connection_num){
	try{
		debug("Entered choose_segment()");
		if (0==pkg_choose_segment(&request_server_pkg, connection_num)){
			return 0;
		}
		debug("choose_segment() goes for proxy-fetcher distfiles");
		// download distfiles as a proxy-fetcher
		if (0==pkg_choose_segment(&proxy_fetcher_pkg, connection_num)){
			return 0;
		}
		return 1;
	}catch(...){
		error_log("Error: segget.cpp : choose_segment()");
		return 1;
	}
}
int download_pkgs(){
	try{
		CURLMsg *msg;
		long L;
		int M, Q, U = -1;
		fd_set R, W, E;
		struct timeval T;

		try{
			cm = curl_multi_init();
			// we can optionally limit the total amount of connections this multi handle uses
			curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, (long)MAX_CONNECTS);
		}catch(...){
			error_log("Error in segget.cpp: download_pkgs() : while calling curl_global_init() and curl_multi_init()");
			return EXIT_FAILURE;
		}

		bool keep_running_flag=true;
		struct timeval prev_connection_activation_cycle_time;
		while (keep_running_flag) {
			// Use free connections to download segments connections
			if (1000>time_left_since(prev_connection_activation_cycle_time)){
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
			debug("After attempt to start connection activation cycle");
			U=stats.active_connections_counter;
			debug("before multi_perform");
			if (CURLM_CALL_MULTI_PERFORM != curl_multi_perform(cm, &U)){
				debug("after multi_perform");
					FD_ZERO(&R);
					FD_ZERO(&W);
					FD_ZERO(&E);
					if (curl_multi_fdset(cm, &R, &W, &E, &M)) {
						error_log("Error: curl_multi_fdset");
						return EXIT_FAILURE;
					}
					if (curl_multi_timeout(cm, &L)) {
						error_log("Error: curl_multi_timeout");
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
							error_log("Error: select ("+toString(M+1)+","+toString(L)+"):"+toString(errno)+": "+strerror(errno));
							return EXIT_FAILURE;
						}
					}
				debug("before multi_info_read");
				while ((msg = curl_multi_info_read(cm, &Q))) {
					debug("inside multi_info_read");
					if (msg->msg == CURLMSG_DONE) {
						Tsegment *current_segment;
						CURL *e = msg->easy_handle;
						curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &current_segment);
						curl_multi_remove_handle(cm, e);
						connection_array[current_segment->connection_num].stop(msg->data.result);
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
void *refresh_tui_screen(void * ){
	while (true){
		ulong time_diff_msecs=time_left_since(stats.previous_time);
		if (time_diff_msecs >= settings.current_speed_time_interval_msecs){
			show_progress(time_diff_msecs);
		};
		stats.show_totals();
		sleep(1);
	}
	return 0;
}

void launch_tui_thread(){
	pthread_t tui_thread;
	int iret1;
	iret1 = pthread_create( &tui_thread, NULL, refresh_tui_screen, (void*) NULL);
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
		iret1 = pthread_create( &proxy_fetcher_server_thread, NULL, run_proxy_fetcher_server, (void*) NULL);
		debug_no_msg("proxy_fetcher_server_thread launched");
	}
}

void launch_request_server_thread(){
	if (settings.request_ip!="none"){
		pthread_t request_server_thread;
		int iret1;
		debug_no_msg("Creating request_server_thread.");
		iret1 = pthread_create( &request_server_thread, NULL, run_request_server, (void*) NULL);
		debug_no_msg("request_server_thread launched");
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

int routine(){
	try{
		signal(SIGABRT,segget_exit);//If program aborts go to assigned function "segget_exit".
		signal(SIGTERM,segget_exit);//If program terminates go to assigned function "segget_exit".
		signal(SIGINT,segget_exit);//If program terminates go to assigned function "segget_exit".
		try{
			gettimeofday(&stats.segget_start_time,NULL);
		}catch(...){
			error_log("Error in stats.cpp: reset_previous_time()");
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
			// This function is NOT thread-safe => call it before any other thread is launched
			curl_global_init(CURL_GLOBAL_ALL);
		}catch(...){
			error_log_no_msg("Error in segget.cpp: routine: while calling curl_global_init()");
		}
		try{
			launch_ui_server_thread();
		}catch(...){
			error_log_no_msg("Error in segget.cpp launch_ui_server() failed");
		}
		try{
			launch_request_server_thread();
		}catch(...){
			error_log_no_msg("Error in segget.cpp launch_proxy_fetcher_server_thread failed");
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
			//show_pkgs();
			stats.show_totals();
		}catch(...){
			//error while showing stats
		}
		try{
			download_pkgs();
		}catch(...){
			//error while downloading_pkgs
		}
		try{
			close(ui_server.server_sockfd);
		}catch(...){
			//error while clossing server_sockfd
		}
		return 0;
	}catch(...)
	{
		perror("Error in segget.cpp: routine()");
	}
	return 1;
}

void start_daemon_mode(){
	try{
		debug("start_daemon_mode()");
		switch (fork())
		{
			case 0:  break; //child
			case -1: error_log("Error in segget.cpp: start_daemon_mode() during first fork()"); //fork error
			default: exit(0); //parent
		}
		struct rlimit _ResourceLimit;
		if (getrlimit(RLIMIT_NOFILE, &_ResourceLimit)!=0) error_log("Error in segget.cpp: start_daemon_mode() during getrlimit()");
		if (_ResourceLimit.rlim_max==0) throw("Error in segget.cpp: start_daemon_mode() - no open file descriptors");
		for (uint i=0;i<_ResourceLimit.rlim_max;i++) close(i);
		if (setsid()==-1) error_log_no_msg("Error in segget.cpp: start_daemon_mode() during setsid()");
		switch (fork()){
			case 0:  break; // child
			case -1: error_log_no_msg("Error in segget.cpp: start_daemon_mode() during second fork()");
			default: exit(0); // parent
		}
		umask(0);
		// prevent unmount problems - switch to root
		if (chdir("/")){
			error_log_no_msg("Error in segget.cpp: start_daemon_mode: can NOT chdir(\"/\")");
		}
		// create descriptors in case someone will use them
		int fileDesc = open("/dev/null", O_RDWR); //stdin
		 //stdout
		if (dup(fileDesc)){
			log_no_msg("Error in segget.cpp: start_daemon_mode: during dup(fileDesc) - stdout");
		}
		//stderr
		if (dup(fileDesc)){
			log_no_msg("Error in segget.cpp: start_daemon_mode: during dup(fileDesc) - stderr");
		}
		log_no_msg("Starting daemon routine");
		routine();
		log_no_msg("Exited daemon routine");
	}catch (...){
		error_log_no_msg("Error in segget.cpp: start_daemon_mode()");
		exit(1);
	}
}

int init_curses(){
	try{
		initscr();
		curs_set(0);
		refresh();
		return 0;
	}catch(...){
		perror("Error in segget.cpp: init_curses()");
	}
	return 1;
}
void show_help(){
	try{
		cout << "Usage: seggetd [OPTIONS]" << endl;
		cout << "OPTIONS:" << endl;
		cout << "--conf-dir=path             Use .conf files from specified dir instead of default /etc/seggetd" << endl;
		cout << "--help                      Show this help" << endl;
		cout << "--no-daemon                 Do NOT switch to daemon mode on start" << endl;
		exit (0);
	}catch(...){
		perror("Error in segget.cpp show_help()");
	}
}

int parse_cli_arguments(int argc, char* argv[]){
	try{
		string option,name,value;
		cout << "argc = " << argc << endl;
		int posEqual;
		for(int i = 0; i < argc; i++){
			cout << "argv[" << i << "] = " << argv[i] << endl; 
			option=argv[i];
			posEqual=option.find('=');
			name  = trim(option.substr(0,posEqual));
			value = trim(option.substr(posEqual+1));
			if (name=="--help") {show_help(); continue;};
			if (name=="--conf-dir") {settings.conf_dir=value; continue;};
			if (name=="--no-daemon"){settings.no_daemon_flag=true; continue;}
		}
		return 0;
	}catch(...){
		perror("Error in segget.cpp: init_curses()");
	}
	return 1;
}

int main(int argc, char* argv[])
{
	try{
		parse_cli_arguments(argc, argv);
		if (settings.no_daemon_flag){
			init_curses();
			routine();
			endwin(); // exit curses
		}else{
			start_daemon_mode();
		}
		exit (0);
	}catch(...){
		perror("Error in segget.cpp main()");
	}
	exit(1);
}