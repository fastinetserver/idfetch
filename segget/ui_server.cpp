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

#include "ui_server.h"

Tui_server ui_server;

void Tui_server::init(){
	socklen_t server_len;
	struct sockaddr_in server_address;
	// Create and name a socket for the server:
		server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
		// Set socket options. We would like the socket to disappear
		// as soon as it has been closed for whatever reason.
		// Set socket options.
		// Allow local port reuse in TIME_WAIT.

		int on=1;
		setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		max_fd_num=server_sockfd;

		server_address.sin_family = AF_INET;
		//server_address.sin_addr.s_addr = htonl(INADDR_ANY);
		string bind_address=settings.ui_ip;
		server_address.sin_addr.s_addr = inet_addr(bind_address.c_str());
		server_address.sin_port = htons(settings.ui_port);
		server_len = sizeof(server_address);
		int res;
		res=bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
		if (res != 0){
				error_log("Error: "+toString(res)
							+" in ui_server.cpp binding socket address "+bind_address
							+":"+toString(ntohs(server_address.sin_port)));
		}
	//Create a connection queue and initialize readfds to handle input from server_sockfd:
	listen(server_sockfd, 5);
	FD_ZERO(&readfds);
	FD_SET(server_sockfd, &readfds);
	send_to_fd_busy=false;
}

//prevent simultaneous writes

ulong Tui_server::send_to_fd(uint fd, string msg){
//	if (send_to_fd_idle) {
	while (send_to_fd_busy){
		sleep(1);
	}
	send_to_fd_busy=true;
	if (fd !=server_sockfd){
		if(FD_ISSET(fd,&ui_server.readfds)) {
			ulong bytes_written=write(fd, msg.c_str(), msg.length());
			if (bytes_written!=msg.length()){
				debug("Error: Not all data has been sent to ui_client()");
			}
		}
	}
	send_to_fd_busy=false;
	return 0;
}

void Tui_server::send_connection_msg_to_fd(uint fd, uint y, string msg){
	string message="<m>c<t>"+toString(y)+"<y>"+msg+"<.>";
	send_to_fd(fd, message);
}

void Tui_server::send_connection_msg_to_all_clients(uint y, string msg){
	string message="<m>c<t>"+toString(y)+"<y>"+msg+"<.>";
	for(uint fd = 0; fd <= ui_server.max_fd_num; fd++){
		send_to_fd(fd, message);
	}
}

void Tui_server::send_log_msg_to_all_clients(string msg){
	string message="<m>l<t>"+msg+"<.>";
	for(uint fd = 0; fd <= ui_server.max_fd_num; fd++){
		send_to_fd(fd, message);
	}
}

void Tui_server::send_distfile_progress_msg_to_fd(uint fd, string msg){
	string message="<m>d<t>"+msg+"<.>";
	send_to_fd(fd, message);
}

void Tui_server::send_distfile_progress_msg_to_all_clients(string msg){
	string message="<m>d<t>"+msg+"<.>";
	for(uint fd = 0; fd <= ui_server.max_fd_num; fd++){
		send_to_fd(fd, message);
	}
}

void Tui_server::send_error_log_msg_to_all_clients(string msg){
	string message="<m>e<t>"+msg+"<.>";
	for(uint fd = 0; fd <= ui_server.max_fd_num; fd++){
		send_to_fd(fd, message);
	}
}

void *run_ui_server(void * ){
	while(1) {
		max_published_screenline_num=0;
		uint fd;
		int nread;
		ui_server.testfds = ui_server.readfds;
		int result;
		//Now wait for clients and requests. Because you have passed a null pointer as the timeout parameter, no timeout will occur. The program will logg an error if select returns a value less than 1:
		result = select(FD_SETSIZE, &ui_server.testfds, (fd_set *)0,
			(fd_set *)0, (struct timeval *) 0);
		if(result < 1) {
			error_log_no_msg("Error in ui_server on select");
			break;
		}
		//Once you know you’ve got activity, you can find which descriptor it’s on by checking each in turn using FD_ISSET:
		for(fd = 0; fd <= ui_server.max_fd_num; fd++) {
			if(FD_ISSET(fd,&ui_server.testfds)) {
				//If the activity is on server_sockfd, it must be a request for a new connection, and you add the associated client_sockfd to the descriptor set:
				if(fd == ui_server.server_sockfd) {
					uint client_sockfd;
					socklen_t client_len;
					struct sockaddr_in client_address;

					client_len = sizeof(client_address);
					client_sockfd = accept(ui_server.server_sockfd,
					(struct sockaddr *)&client_address, &client_len);

					debug("Connected new ui client");

					if (client_sockfd>ui_server.max_fd_num) ui_server.max_fd_num=client_sockfd;

					FD_SET(client_sockfd, &ui_server.readfds);

					// Get this info to catch up!
					for (uint line_num=0; line_num<=max_published_screenline_num;line_num++){
						ui_server.send_connection_msg_to_fd(client_sockfd, line_num, screenlines[line_num]);
						debug_no_msg("Sending to client line:"+toString(line_num)+" "+screenlines[line_num]);
//						ui_server.send(line_num,screenlines[line_num]);
					}
					for (ulong distfile_num=0; distfile_num<request_server_pkg.distfile_count; distfile_num++){
						ui_server.send_distfile_progress_msg_to_fd(client_sockfd, request_server_pkg.Pdistfile_list[distfile_num]->get_distfile_progress_str());
					}
//					if (0==pkg_choose_segment(&proxy_fetcher_pkg, connection_num)){
				
				//If it isn’t the server, it must be client activity. If close is received, the client has gone away, and you remove it from the descriptor set. Otherwise, you “serve” the client as in the previous examples.
				}else{
					ioctl(fd, FIONREAD, &nread);
					if(nread == 0) {
						close(fd);
						FD_CLR(fd, &ui_server.readfds);
						debug("Client parted from fd:"+toString(fd));
					}else{
						char buffer[1000];
						if (nread!=read(fd, &buffer, nread)){
							debug("Not all data has been read from ui_client()");
						}
					}
				}
			}
		}
	}
	return 0;
}
