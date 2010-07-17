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

#include "proxyfetcher.h"

void *run_proxy_fetcher_server(void * ){
	int server_sockfd, client_sockfd;
	socklen_t server_len, client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	int result;
	fd_set readfds, testfds;

	// Create and name a socket for the server:
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	//server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_addr.s_addr = inet_addr(settings.provide_proxy_fetcher_ip.c_str());
	server_address.sin_port = htons(settings.provide_proxy_fetcher_port);
	server_len = sizeof(server_address);

	bind(server_sockfd, (struct sockaddr *)&server_address, server_len);

	//Create a connection queue and initialize readfds to handle input from server_sockfd:
	listen(server_sockfd, 5);

	FD_ZERO(&readfds);
	FD_SET(server_sockfd, &readfds);

	//Now wait for clients and requests. Because you have passed a null pointer as the timeout parameter, no timeout will occur. The program will exit and report an error if select returns a value less than 1:
	while(1) {
		int fd;
		int nread;
		testfds = readfds;

		debug("proxyfether is waiting for connections");
		result = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, (struct timeval *) 0);
//		debug("proxyfether done waiting");

		if(result < 1) {
			error_log("Error in proxyfetcher.cpp : run");
			exit(1);
		}

		//Once you know you’ve got activity, you can find which descriptor it’s on by checking each in turn using FD_ISSET:
		for(fd = 0; fd < FD_SETSIZE; fd++) {
			if(FD_ISSET(fd,&testfds)) {

			//If the activity is on server_sockfd, it must be a request for a new connection, and you add the associated client_sockfd to the descriptor set:

				if(fd == server_sockfd) {
					debug("new client - read");
					client_len = sizeof(client_address);
					client_sockfd = accept(server_sockfd,
					(struct sockaddr *)&client_address, &client_len);
					FD_SET(client_sockfd, &readfds);
					debug("adding client on fd:"+toString(client_sockfd));

					//If it isn’t the server, it must be client activity. If close is received, the client has gone away, and you remove it from the descriptor set. Otherwise, you “serve” the client as in the previous examples.
				}else{
//					debug("old client - read");
					ioctl(fd, FIONREAD, &nread);
					if(nread == 0) {
						close(fd);
						FD_CLR(fd, &readfds);
						debug("removing client on fd:"+toString(fd));
					}else{
						char buffer[100000]="";
						if (nread!=read(fd, &buffer, nread)){
							error_log("Error in proxyfetcher.cpp : run_proxy_fetcher_server(): Not all data has been read from proxy-fetcher-client");
						}
//						debug("serving client - read");
//						debug("serving client on fd"+toString(fd));
						string recv_msg=buffer;
						error_log("Received a msg from the client:"+recv_msg);
						proxy_fetcher_pkg.try_adding_distfile_to_proxy_fetchers_queue(json_tokener_parse(buffer));
					}
				}
			}
		}
	}
}
