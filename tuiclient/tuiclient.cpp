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

void * watch_keyboard_thread_function(void * ptr){
	char * args = (char *) ptr;
	while (true){
		char key=getch();
		switch (key){
			//KEY_DOWN
			case (char)2:mainwindow.down(1);break;
			//KEY_RIGHT
			case (char)5:mainwindow.down(1);break;
			//KEY_UP
			case (char)3:mainwindow.up(1);break;
			//KEY_LEFT
			case (char)4:mainwindow.up(1);break;
			//KEY_PG_DOWN
			case (char)82:mainwindow.down(mainwindow.bottom_screenline_num);break;
			//KEY_PG_UP
			case (char)83:mainwindow.up(mainwindow.bottom_screenline_num);break;
			case 'u':mainwindow.up(1);break;
			case 'U':mainwindow.up(1);break;
			case 'd':mainwindow.down(1);break;
			case 'D':mainwindow.down(1);break;
			case 'q':quit();break;
			case 'Q':quit();break;
			case 'h':mainwindow.help_win.toggle(); mainwindow.show();break;
			case 'H':mainwindow.help_win.toggle(); mainwindow.show();break;
//			default: screenlines[25]=toString((int)key);
		}
	}
	return 0;
}

void * refresh_screen_thread_function(void * ptr){
	char * args = (char *) ptr;
	while (! mainwindow.exit_flag){
			mainwindow.refresh();
		struct timeval delay;
		delay.tv_sec=0;
		delay.tv_usec=300000;
		select(FD_SETSIZE, (fd_set *)0, (fd_set *)0,(fd_set *)0, (struct timeval *) &delay);//&timeout
	}
	return 0;
}

int main()
{
	try{
		try{
			//init curses
			mainwindow.init();
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
				string recv_msg, first_part, msg_text;

				//Create a socket for the client:
				int result=-1;
				ulong attempt_num=1;
				while (result==-1){
					sockfd = socket(AF_INET, SOCK_STREAM, 0);

					//Name the socket, as agreed with the server:
					address.sin_family = AF_INET;
					address.sin_addr.s_addr = inet_addr("127.0.0.1");
					address.sin_port = htons(9999);
					len = sizeof(address);
					//Connect your socket to the server’s socket:
					result = connect(sockfd, (struct sockaddr *)&address, len);
					if(result == -1) {
						mainwindow.set_status("[Connecting... Attempt:"+toString(attempt_num)+". Waiting for 1 sec, before next reconnect.]");
						close(sockfd);
						attempt_num++;
						sleep(1);
					}
				}

				mainwindow.set_status("[Connected]");
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
							read(sockfd, recv_buffer, BUFFER_SIZE);
							//recv_msg=recv_msg+recv_buffer;
							recv_msg=recv_msg+recv_buffer;
							while (recv_msg.find("<.>")!=recv_msg.npos){
								string original_msg=recv_msg;
								recv_msg=recv_msg.substr(recv_msg.find("<y>")+3,recv_msg.npos);
								first_part=recv_msg.substr(0,recv_msg.find("<.>"));
								recv_msg=recv_msg.substr(recv_msg.find("<.>")+3);
								uint line_num=atoi(first_part.substr(0,first_part.find("<s>")).c_str());
								msg_text=first_part.substr(first_part.find("<s>")+3,first_part.npos);
								if (line_num<200){
									mainwindow.set_line(line_num, msg_text);
//									set_line(line_num, "||"+toString(line_num)+"||("+first_part.substr(0,first_part.find("<s>")+3)+")"+msg_text);
//									set_line(line_num, "||"+toString(line_num)+"||("+original_msg);
								}else{
									mainwindow.screen_info_lines[line_num-200]=msg_text;
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