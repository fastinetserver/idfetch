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

#include <iostream>
#include "tuiclient.h"
#include "str.h"


Twindow * next_window(){
	cur_window_num++;
	if (cur_window_num>3) cur_window_num=0;
	switch (cur_window_num){
		case 0:
				mainwindow.log_win.visible=false;
				mainwindow.error_log_win.visible=false;
				mainwindow.distfiles_win.visible=false;
				return &mainwindow;
		case 1:
				mainwindow.log_win.visible=true;
				mainwindow.error_log_win.visible=false;
				mainwindow.distfiles_win.visible=false;
				return &mainwindow.log_win;
		case 2:
				mainwindow.log_win.visible=false;
				mainwindow.error_log_win.visible=true;
				mainwindow.distfiles_win.visible=false;
				return &mainwindow.error_log_win;
		case 3:
				mainwindow.log_win.visible=false;
				mainwindow.error_log_win.visible=false;
				mainwindow.distfiles_win.visible=true;
				return &mainwindow.distfiles_win;
	}
	return &mainwindow;
}

/*
void * watch_keyboard_thread_function(void *){
	Twindow * cur_window=&mainwindow.distfiles_win;
	while (not exit_flag){
		int key=getch();
		if (key==ERR){
			mainwindow.refresh();
			struct timeval delay;
			delay.tv_sec=0;
			delay.tv_usec=300000;
			select(FD_SETSIZE, (fd_set *)0, (fd_set *)0,(fd_set *)0, (struct timeval *) &delay);//&timeout
		}else{
			switch (key){
				//tab - move between windows
				case (char)9:cur_window=next_window();
				//KEY_DOWN
				case (char)2:cur_window->down(1);mainwindow.set_line(25,toString((int)key));break;
				//KEY_RIGHT
				case (char)5:cur_window->down(1);mainwindow.set_line(25,toString((int)key));break;
				//KEY_UP
				case (char)3:cur_window->up(1);mainwindow.set_line(25,toString((int)key));break;
				//KEY_LEFT
				case (char)4:cur_window->up(1);mainwindow.set_line(25,toString((int)key));break;
				//KEY_PG_DOWN
				case (char)82:cur_window->down(mainwindow.bottom_screenline_num);break;
				//KEY_PG_UP
				case (char)83:cur_window->up(mainwindow.bottom_screenline_num);break;
				case 'd':
				case 'D':mainwindow.distfiles_win.toggle();
						mainwindow.log_win.visible=false;
						mainwindow.error_log_win.visible=false;
						cur_window=&mainwindow.distfiles_win;
						mainwindow.show();
						break;
				case 'q':
				case 'Q':quit(0,"Interrupted by user");break;
				case 'h':
				case 'H':mainwindow.help_win.toggle(); mainwindow.show();break;
				case 'l':
				case 'L':mainwindow.log_win.toggle(); mainwindow.show();break;
				case 'e':
				case 'E':mainwindow.error_log_win.toggle(); mainwindow.show();break;
				default: mainwindow.set_line(25,toString((int)key));
			}
		}
	}
//	pthread_exit((void*)watch_keyboard_thread_status);
	exit_curses_and_show_msg();
	return (void*)0;
	//watch_keyboard_thread_status;
}

void * refresh_screen_thread_function(void *){
	return (void*)0;
}
*/

void * watch_keyboard_thread_function(void *){
	Twindow * cur_window=&mainwindow.distfiles_win;
	while (not exit_flag){
		char key=getch();
		switch (key){
			//tab - move between windows
			case (char)9:cur_window=next_window();
			//KEY_DOWN
			case (char)2:cur_window->down(1);break;
			//KEY_RIGHT
			case (char)5:cur_window->down(1);break;
			//KEY_UP
			case (char)3:cur_window->up(1);break;
			//KEY_LEFT
			case (char)4:cur_window->up(1);break;
			//KEY_PG_DOWN
			case (char)82:cur_window->down(mainwindow.bottom_screenline_num);break;
			//KEY_PG_UP
			case (char)83:cur_window->up(mainwindow.bottom_screenline_num);break;
			case 'd':
			case 'D':mainwindow.distfiles_win.toggle();
					mainwindow.log_win.visible=false;
					mainwindow.error_log_win.visible=false;
					cur_window=&mainwindow.distfiles_win;
					mainwindow.show();
					break;
			case 'q':
			case 'Q':quit(0,"Interrupted by user");break;
			case 'h':
			case 'H':mainwindow.help_win.toggle(); mainwindow.show();break;
			case 'l':
			case 'L':mainwindow.log_win.toggle(); mainwindow.show();break;
			case 'e':
			case 'E':mainwindow.error_log_win.toggle(); mainwindow.show();break;
			default: mainwindow.set_line(25,toString((int)key));
		}
	}
//	pthread_exit((void*)watch_keyboard_thread_status);
	return (void*)watch_keyboard_thread_status;
}

void * refresh_screen_thread_function(void *){
	while (! exit_flag){
		mainwindow.refresh();
		mainwindow.refresh();
//		sleep(1);
		struct timeval delay;
		delay.tv_sec=0;
		delay.tv_usec=300000;
		select(FD_SETSIZE, (fd_set *)0, (fd_set *)0,(fd_set *)0, (struct timeval *) &delay);//&timeout
	}
	exit_curses_and_show_msg();
	return 0;
}

void decode_connection_msg(string msg_body){
	Tparts parts=split("<y>",msg_body);
	int line_num=atoi(parts.before.c_str());
	if (line_num<200){
		mainwindow.set_line(line_num, parts.after);
//		set_line(line_num, "||"+toString(line_num)+"||("+first_part.substr(0,first_part.find("<s>")+3)+")"+msg_text);
//		set_line(line_num, "||"+toString(line_num)+"||("+original_msg);
	}else{
		mainwindow.screen_info_lines[line_num-200]=parts.after;
	}
}
/*

void decode_downloaded_distfile_msg(string msg_body){
	vector <string> parts;
	parts=split_to_vector(" ", msg_body);
	if (parts.size()>4){
//		if (settings.arg_wait_distfile!=""){
//			if (parts[0]==settings.arg_wait_distfile){
//				cant_find_distfile_exit_flag==false;
//				if
//			}
//		}
		for (ulong cur_distfile=0;cur_distfile<tuidistfiles.size();cur_distfile++){
			if (tuidistfiles[cur_distfile].name==parts[0]){
				tuidistfiles[cur_distfile].dld_segments=atol(parts[1]);
				tuidistfiles[cur_distfile].segments_count=atol(parts[2]);
				tuidistfiles[cur_distfile].dld_bytes=atol(parts[3]);
				tuidistfiles[cur_distfile].size=atol(parts[4]);
				return;
			}
		}
		Ttuidistfile a_tuidistfile;
		a_tuidistfile.name=parts[0];
		a_tuidistfile.dld_segments=atol(parts[1]);
		a_tuidistfile.segments_count=atol(parts[2]);
		a_tuidistfile.dld_bytes=atol(parts[3]);
		a_tuidistfile.size=atol(parts[4]);
		tuidistfiles.push_back(a_tuidistfile);
		mainwindow.distfiles_win.max_received_screenline_num=tuidistfiles.size();
	}
}
*/

void decode_downloaded_distfile_msg(string msg_body){
	vector <string> parts;
	parts=split_to_vector("<>", msg_body);
	if (parts.size()>5){
		for (ulong cur_distfile=0;cur_distfile<tuidistfiles.size();cur_distfile++){
			if (tuidistfiles[cur_distfile].name==parts[0]){
				tuidistfiles[cur_distfile].status=(Tdistfile_status)atoi(parts[1]);
				tuidistfiles[cur_distfile].dld_segments=atol(parts[2]);
				tuidistfiles[cur_distfile].segments_count=atol(parts[3]);
				tuidistfiles[cur_distfile].dld_bytes=atol(parts[4]);
				tuidistfiles[cur_distfile].size=atol(parts[5]);
				return;
			}
		}
		Ttuidistfile a_tuidistfile;
		a_tuidistfile.name=parts[0];
		a_tuidistfile.status=(Tdistfile_status)atoi(parts[1]);
		a_tuidistfile.dld_segments=atol(parts[2]);
		a_tuidistfile.segments_count=atol(parts[3]);
		a_tuidistfile.dld_bytes=atol(parts[4]);
		a_tuidistfile.size=atol(parts[5]);
		tuidistfiles.push_back(a_tuidistfile);
	}
}
		
void decode_log_msg(string msg_body){
	mainwindow.log_win.add_line(msg_body);
}

void decode_error_log_msg(string msg_body){
	mainwindow.error_log_win.add_line(msg_body);
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
			if (name=="--wait-distfile"){
				settings.arg_wait_distfile=value;
				continue;
			};
//		//f (name=="--pkglist-file") {settings.pkg_list_file=value; continue;};
		}
		return 0;
	}catch(...){
		perror("Error in segget.cpp: init_curses()");
	}
	return 1;
}

void no_curses_msg(string msg){
	cout << msg << endl;
}

void exit_curses_and_show_msg(){
	try{
		nocbreak();
		endwin();
		try{
			close(sockfd);
		}catch(...){
			no_curses_msg("error while closing socket");
		}
		no_curses_msg(exit_msg);
		exit(exit_code);
	}catch(...){
		//error while ending curses
	}
}

void quit(int exit_code_, string exit_msg_){
//		mainwindow.exit_flag=TRUE;
		exit_code=exit_code_;
		exit_msg=exit_msg_;
		exit_flag=true;
		//wait to exit
		while (1){
			sleep(10);
		}
	//		nodelay(mainwindow.window,0);
//		sleep(1); // wait for thread to finish its job
//		void *status;
//		pthread_join(watch_keyboard_thread, &status);
//		if ((int)status != watch_keyboard_thread_status) {
//			printf("Secondary thread failed\n");
//		exit(1);
//		}
//		pthread_join(watch_keyboard_thread, 0);
}

int main(int argc, char* argv[])
{
	try{
		parse_cli_arguments(argc, argv);
	}catch(...){
		perror("error while parsing arguments");
	}
	try{
		try{
			//init curses
			settings.init();
			mainwindow.init();
//			init_color(COLOR_BLUE, 0, 0, 0); 
		}catch(...){
			//error while init curses
		}
		try{
			pthread_t refresh_screen_thread;
			int iret1;
			iret1 = pthread_create( &refresh_screen_thread, NULL, refresh_screen_thread_function, (void*) NULL);
		}catch(...){
			//error while init refresh_screen_thread;
		}
		try{
			int iret1;
			iret1 = pthread_create( &watch_keyboard_thread, NULL, watch_keyboard_thread_function, (void*) NULL);
		}catch(...){
			//error while init curses watch_keyboard_thread;
		}
		try{
//			struct timeval timeout;
			while ( ! exit_flag ){
				int len;

				struct sockaddr_in address;
				string rest_of_the_msg, first_part, msg_text;

				//Create a socket for the client:
				int result=-1;
				while (result==-1){
					mainwindow.attempt_num++;
					sockfd = socket(AF_INET, SOCK_STREAM, 0);

					//Name the socket, as agreed with the server:
					address.sin_family = AF_INET;
					address.sin_addr.s_addr = inet_addr(settings.ui_ip.c_str());
					address.sin_port = htons(settings.ui_port);
					len = sizeof(address);
					//Connect your socket to the server’s socket:
					result = connect(sockfd, (struct sockaddr *)&address, len);
					if(result == -1) {
						mainwindow.disconnected();
						close(sockfd);
						sleep(1);
					}
				}

				debug("connected");
				mainwindow.connected();
				if (settings.arg_wait_distfile.length()>999){
					quit(1,"Error in argument --wait-distfile="+settings.arg_wait_distfile+" : Distfile name is too long");
				};
				char send_buffer[10000];
				string send_msg="<d>"+settings.arg_wait_distfile+"<.>";
				strcpy(send_buffer,send_msg.c_str());
				if (write(sockfd, send_buffer, strlen(send_buffer))!=(int)send_msg.length()){
					perror("Error in request.cpp: request(): request msg size and sent data size are different.");
				}

				fd_set readfds, testfds;

				FD_ZERO(&readfds);
				FD_SET(sockfd, &readfds);
				testfds = readfds;
				bool run_flag=true;
				while (run_flag){
					//init each time !
//					timeout.tv_sec=1;
//					timeout.tv_usec=0;
					result = select(FD_SETSIZE, &testfds, (fd_set *)0,
						(fd_set *)0, (struct timeval *) 0);//&timeout
					// if fd is set - it's not because of timeout - read data
					if (FD_ISSET(sockfd, &testfds)){
						int nread;
						ioctl(sockfd, FIONREAD, &nread);
						if(nread == 0) {
							close(sockfd);
//							FD_CLR(sockfd, &readfds);
//							printf("removing client on fd %d\n", sockfd);
							run_flag=false;
						}else {
							char recv_buffer[BUFFER_SIZE+1]=" ";
							if (nread!=read(sockfd, recv_buffer, BUFFER_SIZE)){
								error_log("Error in tuiclient.cpp : main() read bytes count does NOT match declared count.");
							};
							//recv_msg=recv_msg+recv_buffer;
//							rest_of_the_msg=rest_of_the_msg+recv_buffer;
							rest_of_the_msg=recv_buffer;
							debug("RECEIVED:"+rest_of_the_msg);
							debug("END============================");
							Tparts msg_parts;
							vector<string> submsgs=split_to_vector("<.>", rest_of_the_msg);
							for (ulong submsg_num=0; submsg_num<submsgs.size(); submsg_num++){
								vector<string> before_msg_and_msg=split_to_vector("<m>",submsgs[submsg_num]);
								if (before_msg_and_msg.size()>1){
									vector<string> msg_type_and_body=split_to_vector("<t>",before_msg_and_msg[1]);
									if (msg_type_and_body.size()>1){
										debug("msg_type="+msg_type_and_body[0]);
										debug("msg_body="+msg_type_and_body[1]);
										char msg_type=msg_type_and_body[0][0];
//										debug("msg_type="+msg_type_str);
										switch (msg_type){
											case 'c': decode_connection_msg(msg_type_and_body[1]); break;
											case 'l': decode_log_msg(msg_type_and_body[1]);break;
											case 'e': decode_error_log_msg(msg_type_and_body[1]);break;
											case 'd': decode_downloaded_distfile_msg(msg_type_and_body[1]);break;
											// yes-distfile present in the queue
											case 'y': break; // continue waiting for catchup info
											// no distfile in the queue
											case 'n': 
												quit(1,"Distfile: "+settings.arg_wait_distfile+" is not in the queue - quit");
												break; //shouldn't get to this point - but just in case
											case 'N': 
												quit(0,"Distfile: "+settings.arg_wait_distfile+" is already downloaded - quit");
												break; //shouldn't get to this point - but just in case
										}
									}
								}
							}
						};
					}
				}
				close(sockfd);
			}
		}catch(...){
//			error_log_no_msg("Error in segget.cpp launch_tui_theread() failed");
		}
	
	}catch(...){
		perror("error during init and downloading process");
	}
	while (1){
		sleep(10);
	}
	quit(0,"");
}