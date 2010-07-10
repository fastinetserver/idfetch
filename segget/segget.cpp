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

int choose_segment(uint connection_num){
	try{
		uint pkg_num (0);
		uint distfile_num(0);
		uint segment_num(0);
		while (pkg_num<stats.pkg_count){
//			debug("pkg_num:"+toString(pkg_num));
			while(distfile_num<Ppkg_array[pkg_num]->distfile_count){
				if (Ppkg_array[pkg_num]->Pdistfile_list[distfile_num]->allows_new_actions()){
					debug("Distfile "+Ppkg_array[pkg_num]->Pdistfile_list[distfile_num]->name+" allows new connections");
//					debug("	distfile_num:"+toString(distfile_num));
					if (Ppkg_array[pkg_num]->Pdistfile_list[distfile_num]->active_connections_num<settings.max_connection_num_per_distfile){
						debug("max_connection limit has not been reached");
						debug("segment_num:"+toString(segment_num));
						debug("segment_count:"+toString(Ppkg_array[pkg_num]->Pdistfile_list[distfile_num]->segments_count));
						while (segment_num<Ppkg_array[pkg_num]->Pdistfile_list[distfile_num]->segments_count){
							debug("segment_num:"+toString(segment_num));
//							debug("		segment_num:"+toString(segment_num));
							//	segments_in_progress[connection_num]=
							//	if not(Ppkg_array[pkg_num]->Pdistfile_list[distfile_num]->get_segment_downloaded_status(segment_num);
							debug("Let's get segment status"+statusToString(Ppkg_array[pkg_num]->Pdistfile_list[distfile_num]->dn_segments[segment_num].status));
							if (Ppkg_array[pkg_num]->Pdistfile_list[distfile_num]->dn_segments[segment_num].status==SWAITING){
								if ( ! Ppkg_array[pkg_num]->Pdistfile_list[distfile_num]->provide_segment(cm, connection_num, segment_num)){
									return 0; // success
								};
							}else{
								debug("status is not SWAITING - go for the next segment");
							}
							// haven't managed to provide this segment, go for the next one
							segment_num++;
						}
					}else{
							debug("	distfile "+Ppkg_array[pkg_num]->Pdistfile_list[distfile_num]->name+" has "
							+toString(Ppkg_array[pkg_num]->Pdistfile_list[distfile_num]->active_connections_num)
							+" connections => choosing another distfile.");
					}
				}
				distfile_num++;
				segment_num=0;
			}
			distfile_num=0;
			pkg_num++;
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
		while (keep_running_flag){
			U=1;
			while (U) {
				// Use free connections to download segments connections
				for (uint connection_num = 0; connection_num < settings.max_connections; ++connection_num) {
					debug("connection_num:"+toString(connection_num));
					if ( ! connection_array[connection_num].active){
//						if (
						debug("connection is not active - choosing segment");
						choose_segment(connection_num);
//						    )
//							break;
					}
					else{
						debug("connection is active");
					}
				};
				while (CURLM_CALL_MULTI_PERFORM == curl_multi_perform(cm, &U)){};
				if (U) {
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
				}
				while ((msg = curl_multi_info_read(cm, &Q))) {
					if (msg->msg == CURLMSG_DONE) {
						Tsegment *current_segment;
						CURL *e = msg->easy_handle;
						curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &current_segment);
						uint connection_result=msg->data.result;
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
			debug("bbbbbblllllllllllllaaaaaaaaaaaaa");
			// nothing to download - wait 5 secs and check upper_proxy_fetchers again
			// later will be replaced by server_waiting_for_incoming connections and new tasks
			// with timeout to check downloads from upper proxy fetcher
			sleep(1);
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

void *print_message_function( void *ptr );

int main()
{
	try{
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
		pthread_t thread1;
//			, thread2;
//		char *message1 = "Thread 1";
//		char *message2 = "Thread 2";
		int iret1;
//		int iret2;

    /* Create independent threads each of which will execute function */

		iret1 = pthread_create( &thread1, NULL, print_message_function, (void*) NULL);
//		iret2 = pthread_create( &thread2, NULL, print_message_function, (void*) message2);

		/* Wait till threads are complete before main continues. Unless we  */
		/* wait we run the risk of executing an exit which will terminate   */
		/* the process and all threads before the threads have completed.   */

	debug("oooooooooooooo111111111111111111");
//		pthread_join( thread1, NULL);
	debug("oooooooooooooo2222222222222222222222");
//		pthread_join( thread2, NULL); 

//     printf("Thread 1 returns: %d\n",iret1);
//     printf("Thread 2 returns: %d\n",iret2);

		
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
	return 0;
}

void *print_message_function(void *ptr){
//	char *message;
	char * args;
	args = (char *) ptr;
	ulong i=0;
	while (true){
		ulong time_diff_msecs=time_left_from(stats.previous_time);
		if (time_diff_msecs >= settings.current_speed_time_interval_msecs){
			show_progress(time_diff_msecs);
		};
		log("Thread:"+toString(i));
			//haven't downloaded anything - anyway, show totals
			stats.show_totals();
		sleep(1);
		i++;
	}
	return 0;
//     printf("%s \n", message);
}