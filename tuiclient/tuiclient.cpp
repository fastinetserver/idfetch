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

#include "tuiclient.h"

void quit(){
	mainwindow.exit_flag=TRUE;
	try{
		close(sockfd);
	}
	catch(...)
	{
		//error while closing socket
	}
	try{
		close(sockfd);
//		sleep(1);
		nocbreak();
		endwin();
		exit(0);
	}
	catch(...)
	{
		//error while ending curses
	}
}
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


void * watch_keyboard_thread_function(void *){
	Twindow * cur_window=&mainwindow.distfiles_win;
	while (true){
		char key=getch();
		switch (key){
			//tab - move between windows
			case (char)9:cur_window=next_window();
			//KEY_DOWN
			case (char)2:cur_window->down(1);mainwindow.screenlines[25]=toString((int)key);break;
			//KEY_RIGHT
			case (char)5:cur_window->down(1);mainwindow.screenlines[25]=toString((int)key);break;
			//KEY_UP
			case (char)3:cur_window->up(1);mainwindow.screenlines[25]=toString((int)key);break;
			//KEY_LEFT
			case (char)4:cur_window->up(1);mainwindow.screenlines[25]=toString((int)key);break;
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
			case 'Q':quit();break;
			case 'h':
			case 'H':mainwindow.help_win.toggle(); mainwindow.show();break;
			case 'l':
			case 'L':mainwindow.log_win.toggle(); mainwindow.show();break;
			case 'e':
			case 'E':mainwindow.error_log_win.toggle(); mainwindow.show();break;
			default: mainwindow.screenlines[25]=toString((int)key);
		}
	}
	return 0;
}

void * refresh_screen_thread_function(void *){
	while (! mainwindow.exit_flag){
			mainwindow.refresh();
		struct timeval delay;
		delay.tv_sec=0;
		delay.tv_usec=300000;
		select(FD_SETSIZE, (fd_set *)0, (fd_set *)0,(fd_set *)0, (struct timeval *) &delay);//&timeout
	}
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

void decode_downloaded_distfile_msg(string msg_body){
	vector <string> parts;
	parts=split_to_vector(" ", msg_body);
	if (parts.size()>4){
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
		
void decode_log_msg(string msg_body){
	mainwindow.log_win.add_line(msg_body);
}

void decode_error_log_msg(string msg_body){
	mainwindow.error_log_win.add_line(msg_body);
}

int main()
{
	try{
		try{
			//init curses
			settings.init();
			mainwindow.init();
//			init_color(COLOR_BLUE, 0, 0, 0); 

		}catch(...)
		{
			//error while init curses
		}
		try{
			pthread_t watch_keyboard_thread;
			int iret1;
			iret1 = pthread_create( &watch_keyboard_thread, NULL, watch_keyboard_thread_function, (void*) NULL);
		}catch(...)
		{
			//error while init curses watch_keyboard_thread;
		}
		try{
			pthread_t refresh_screen_thread;
			int iret1;
			iret1 = pthread_create( &refresh_screen_thread, NULL, refresh_screen_thread_function, (void*) NULL);
		}catch(...)
		{
			//error while init refresh_screen_thread;
		}
		try{
//			struct timeval timeout;
			while ( ! mainwindow.exit_flag ){
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

				
				mainwindow.connected();
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
							char recv_buffer[BUFFER_SIZE+1]="";
							if (nread!=read(sockfd, recv_buffer, BUFFER_SIZE)){
								error_log("Error in tuiclient.cpp : main() read bytes count does NOT match declared count.");
							};
							//recv_msg=recv_msg+recv_buffer;
							rest_of_the_msg=rest_of_the_msg+recv_buffer;
							Tparts msg_parts;
							while (rest_of_the_msg.find("<.>")!=rest_of_the_msg.npos){
								msg_parts=split("<m>",rest_of_the_msg);
								msg_parts=split("<t>",msg_parts.after);
								char msg_type=msg_parts.before[0];
								debug("msg_type="+msg_type);
								msg_parts=split("<.>",msg_parts.after);
								string msg_body=msg_parts.before;
								debug("msg_body="+msg_body);
								rest_of_the_msg=msg_parts.after;
								switch (msg_type){
									case 'c': decode_connection_msg(msg_parts.before); break;
									case 'l': decode_log_msg(msg_parts.before);break;
									case 'e': decode_error_log_msg(msg_parts.before);break;
									case 'd': decode_downloaded_distfile_msg(msg_parts.before);break;
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
	}
	catch(...){
		//error during init and downloading process
	}
	try{
		nocbreak();
		endwin();
	}
	catch(...)
	{
		//error while ending curses
	}
	return 0;
}