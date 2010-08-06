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

enum TDFsearch_rusults{
	NOT_FOUND,
	DOWNLOADED,
	IN_QUEUE
};

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

ulong Tui_server::send_binary_to_fd(uint fd, string image_file_name){
//	if (send_to_fd_idle) {
			ifstream image_file;
			try{
				image_file.open (image_file_name.c_str(), ios::in|ios::binary|ios::ate);
	//			file.open((settings.conf_dir+"/"+config_file_name).c_str());
			}
			catch(...){
				error_log("Error opening image file: ");
	//			return;
			}
			try{
				//processing file
				ifstream::pos_type size;
				char * memblock;
				if (image_file.is_open()){
					size = image_file.tellg();
					memblock = new char [size];
					image_file.seekg (0, ios::beg);
					image_file.read (memblock, size);

					while (send_to_fd_busy){
						sleep(1);
					}
					send_to_fd_busy=true;
					if (fd !=server_sockfd){
						if(FD_ISSET(fd,&ui_server.readfds)) {
							ulong bytes_written=write(fd, memblock, size);
							if (bytes_written!=size){
								debug("Error: Not all data has been sent to ui_client during send_binary_to_fd()");
							}
						}
					}
					send_to_fd_busy=false;
					image_file.close();
					delete[] memblock;
				}
			}catch(ifstream::failure e){
//
			}catch(...){
	//			error_log("Settings file: "+config_file_name+" was opened, but an error occured while reading settings from it.");
			}

	return 0;
}


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
		if FD_ISSET(fd, &tuiclient_fds_set){
			send_to_fd(fd, message);
		}
	}
}

void Tui_server::send_log_msg_to_all_clients(string msg){
	string message="<m>l<t>"+msg+"<.>";
	for(uint fd = 0; fd <= ui_server.max_fd_num; fd++){
		if FD_ISSET(fd, &tuiclient_fds_set){
			send_to_fd(fd, message);
		}
	}
}

void Tui_server::send_distfile_progress_msg_to_fd(uint fd, string msg){
	string message="<m>d<t>"+msg+"<.>";
	send_to_fd(fd, message);
}

void Tui_server::send_distfile_progress_msg_to_all_clients(string msg){
	string message="<m>d<t>"+msg+"<.>";
	for(uint fd = 0; fd <= ui_server.max_fd_num; fd++){
		if FD_ISSET(fd, &tuiclient_fds_set){
			send_to_fd(fd, message);
		}
	}
}

void Tui_server::send_error_log_msg_to_all_clients(string msg){
	string message="<m>e<t>"+msg+"<.>";
	for(uint fd = 0; fd <= ui_server.max_fd_num; fd++){
		if FD_ISSET(fd, &tuiclient_fds_set){
			send_to_fd(fd, message);
		}
	}
}

void Tui_server::serve_tuiclient(uint fd, string msg){
	try{
		debug("tuiclient connected");
		string request_str_before,request_str_after;
		split("<d>",msg,request_str_before,request_str_after);
		split("<.>",request_str_after,request_str_before,request_str_after);
		string distfile_by_name_lookup_request=request_str_before;
		TDFsearch_rusults distfile_search_result=NOT_FOUND;
		if (distfile_by_name_lookup_request.length()>0){
			for (ulong distfile_num=0; distfile_num<request_server_pkg.distfile_count; distfile_num++){
				if (distfile_by_name_lookup_request==request_server_pkg.Pdistfile_list[distfile_num]->name){
					if (request_server_pkg.Pdistfile_list[distfile_num]->get_status()==DDOWNLOADED){
						distfile_search_result=DOWNLOADED;
					}else{
						distfile_search_result=IN_QUEUE;
					}
					break;
				}
			}
			if (distfile_search_result==NOT_FOUND){
				for (ulong distfile_num=0; distfile_num<proxy_fetcher_pkg.distfile_count; distfile_num++){
					if (distfile_by_name_lookup_request==proxy_fetcher_pkg.Pdistfile_list[distfile_num]->name){
						if (proxy_fetcher_pkg.Pdistfile_list[distfile_num]->get_status()==DDOWNLOADED){
							distfile_search_result=DOWNLOADED;
						}else{
							distfile_search_result=IN_QUEUE;
						}
						break;
					}
				}
			}
		}else{
			// if no name for distfile specified -> no need to find distfile
			// just keep an eye on the ones in queue
			distfile_search_result=IN_QUEUE;
		}
		switch (distfile_search_result){
			case NOT_FOUND:
				ui_server.send_to_fd(fd, "<m>n<t><.>"); //distfile is not in the list quit
				break;
			case DOWNLOADED:
				ui_server.send_to_fd(fd, "<m>N<t><.>"); //distfile is not in the list quit
				break;
			case IN_QUEUE:
				ui_server.send_to_fd(fd, "<m>y<t><.>"); //distfile is in the list continue
				// Get this info to catch up!
				for (uint line_num=0; line_num<=max_published_screenline_num;line_num++){
					ui_server.send_connection_msg_to_fd(fd, line_num, screenlines[line_num]);
					debug("Sending to client line:"+toString(line_num)+" "+screenlines[line_num]);
				}
				debug("Sending to client distfiles_num:"+toString(request_server_pkg.Pdistfile_list.size()));
				for (ulong distfile_num=0; distfile_num<request_server_pkg.Pdistfile_list.size(); distfile_num++){
					ui_server.send_distfile_progress_msg_to_fd(fd, request_server_pkg.Pdistfile_list[distfile_num]->get_distfile_progress_str());
					debug("Sending to client:"+request_server_pkg.Pdistfile_list[distfile_num]->get_distfile_progress_str());
				}
				for (ulong distfile_num=0; distfile_num<proxy_fetcher_pkg.Pdistfile_list.size(); distfile_num++){
					ui_server.send_distfile_progress_msg_to_fd(fd, proxy_fetcher_pkg.Pdistfile_list[distfile_num]->get_distfile_progress_str());
					debug("Sending to client:"+proxy_fetcher_pkg.Pdistfile_list[distfile_num]->get_distfile_progress_str());
				}
		}
	}catch(...){
		error_log_no_msg("Error in ui_server.cpp: serve_server()");
	}
}

string Tui_server::serve_browser_distfile_progress(Tdistfile * a_distfile){
	try{
		uint percent;
		if (a_distfile->size>0){
			percent=a_distfile->dld_bytes*100/a_distfile->size;
		}else{
			percent=50;
		}
		string result=(string)"<td>"
				+"<img src=\"/img/blue_progress.jpg\" alt=\"blue_progress\" height=20 width="+toString(percent)+"/>"
				+"<img src=\"/img/bw_progress.jpg\" alt=\"bw_progress\" height=20 width="+toString(100-percent)+" />"
				+"</td><td align=center>"+toString(percent)+"%"
				+"</td><td>"+a_distfile->name
				+"</td><td align=center bgcolor=\""+a_distfile->statusToColor()+"\">"+a_distfile->statusToString()
				+"</td><td align=right>"+toString(a_distfile->dld_segments_count)
				+"</td><td align=right>"+toString(a_distfile->segments_count)
				+"</td><td align=right>"+toString(a_distfile->dld_bytes)
				+"</td><td align=right>"+toString(a_distfile->size)
				+"</td>";
		return result;
	}catch(...){
		error_log("Error: ui_server.cpp: serve_browser_distfile_progress()");
		return "";
	}
}

string Tui_server::get_header(string title){
	string header;
	header=(string)"HTTP/1.0 200 OK\n\n"
		+"<html><head><meta http-equiv=\"refresh\" content=\"1\" ><title> Segget - "+title+"</title></head>"
		+"<body>"
		+"<table border=0>"
			+"<tr align=center>"
				+"<td><a href=\"connections\"><img src=\"/img/connections.jpg\" alt=\"Segments\" height=50 width=50/></a></td>"
				+"<td><a href=\"distfiles\"><img src=\"/img/distfiles.png\" alt=\"Distfiles\" height=50 width=50/></a></td>"
				+"<td><a href=\"log\"><img src=\"/img/log.png\" alt=\"Log\" height=50 width=50/></a></td>"
				+"<td><a href=\"errors_log\"><img src=\"/img/errors_log.jpg\" alt=\"Errors log\" height=50 width=50/></a></td>"
			+"</tr>"
			+"<tr align=center>"
				+"<td><a href=\"connections\">Connections</a></td>"
				+"<td><a href=\"distfiles\">Distfiles</a></td>"
				+"<td><a href=\"log\">Log</a></td>"
				+"<td><a href=\"errors_log\">Errors log</a></td>"
		+"</tr>"
		+"</table>"
		+"<h1>"+title+"</h1>";
	return header;
}

string Tui_server::get_footer(){
	return "</body></html>";
}

string Tui_server::get_connections_info(){
	try{
		string result=(string)"<center>"
			+"<h3>Active connections: "+toString(stats.active_connections_counter)+"/"+toString(settings.max_connections)+"</h1>"
		+"<table border=1 width=100%>"
			+"<tr>"
				+"<th rowspan=2>Distfile progress</th>"
				+"<th rowspan=2>Distfile name</th>"
				+"<th rowspan=2>Segment progress</th>"
				+"<th rowspan=2>Segment #</th>"
				+"<th rowspan=2>Try</th>"
				+"<th colspan=2>Network</th>"
				+"<th colspan=2>Bytes</th>"
				+"<th colspan=2>Speed</th>"
				+"<th rowspan=2 width=100>ETA</th>"
			+"</tr>"
			+"<tr>"
				+"<th>Num</th>"
				+"<th>Type</th>"
				+"<th>Downloaded</th>"
				+"<th>Total</th>"
				+"<th>Current</th>"
				+"<th>Average</th>"
			+"</tr>";
		for (uint connection_num = 0; connection_num < settings.max_connections; ++connection_num) {
//			debug("connection_num:"+toString(connection_num));
			if (connection_array[connection_num].active){
				result=result+"<tr>"+connection_array[connection_num].get_html_connection_progress()+"</tr>";
			}
		}
		result=result+"</table></center>";
		return result;
	}catch(...){
		error_log("Error: ui_server.cpp: serve_browser_distfile_progress()");
		return "";
	}
}
void Tui_server::serve_browser(uint fd, string msg){
	try{
		debug("Web browser connected");
		uint pos_uri_start=msg.find("GET");
		uint pos_uri_end=msg.find("HTTP",pos_uri_start);
		string uri;
		if (pos_uri_end!=msg.npos){
			uri=msg.substr(pos_uri_start+4,pos_uri_end-pos_uri_start-5);
			debug("Web browser requests URI:"+uri);
		}
		if (uri=="/connections"){
			send_to_fd(fd,get_header("Connections"));
			send_to_fd(fd,get_connections_info());
			send_to_fd(fd,get_footer());
		}else if (uri=="/log"){
			ui_server.send_to_fd(fd,get_header("Log"));
			ui_server.send_to_fd(fd,"<center><table border=\"1\" width=\"100%\">");
			for (uint log_line_num=0; log_line_num<log_lines.size(); log_line_num++){
				ui_server.send_to_fd(fd,"<tr><td>"+log_lines[log_line_num]+"</td></tr>");
			}
			ui_server.send_to_fd(fd,"</table></center>");
			ui_server.send_to_fd(fd,get_footer());
		}else if (uri=="/errors_log"){
			ui_server.send_to_fd(fd,get_header("Errors log"));
			ui_server.send_to_fd(fd,"<center><table border=\"1\" width=\"100%\">");
			for (uint error_log_line_num=0; error_log_line_num<error_log_lines.size(); error_log_line_num++){
				ui_server.send_to_fd(fd,"<tr><td>"+error_log_lines[error_log_line_num]+"</td></tr>");
			}
			ui_server.send_to_fd(fd,"</table></center>");
			ui_server.send_to_fd(fd,get_footer());
		}else if (uri=="/distfiles"){
			ui_server.send_to_fd(fd,get_header("Distfiles"));
			ui_server.send_to_fd(fd,"<center><table border=\"1\" width=\"100%\">");
				debug("Sending to client distfiles_num:"+toString(request_server_pkg.Pdistfile_list.size()));
				ui_server.send_to_fd(fd,(string)"<tr>"
				+"</th><th rowspan=2>Progress"
				+"</th><th rowspan=2>Percent"
				+"</th><th rowspan=2>Name"
				+"</th><th rowspan=2>Status"
				+"</th><th colspan=2>Segments"
				+"</th><th colspan=2>Bytes"
				+"</th></tr>");
				ui_server.send_to_fd(fd,(string)"<tr>"
				+"</th><th>Downloaded"
				+"</th><th>Total"
				+"</th><th>Downloaded"
				+"</th><th>Total"
				+"</th></tr>");
				for (ulong distfile_num=0; distfile_num<request_server_pkg.Pdistfile_list.size(); distfile_num++){
					ui_server.send_to_fd(fd,"<tr>"+serve_browser_distfile_progress(request_server_pkg.Pdistfile_list[distfile_num])+"</tr>");
					debug("Sending to client:"+request_server_pkg.Pdistfile_list[distfile_num]->get_distfile_progress_str());
				}
				for (ulong distfile_num=0; distfile_num<proxy_fetcher_pkg.Pdistfile_list.size(); distfile_num++){
					ui_server.send_to_fd(fd,"<tr>"+serve_browser_distfile_progress(proxy_fetcher_pkg.Pdistfile_list[distfile_num])+"</tr>");
					debug("Sending to client:"+proxy_fetcher_pkg.Pdistfile_list[distfile_num]->get_distfile_progress_str());
				}
			ui_server.send_to_fd(fd,"</table></center>");
			ui_server.send_to_fd(fd,get_footer());
		}else if (uri=="/favicon.ico"){
			ui_server.send_binary_to_fd(fd,"./webui/img/favicon.ico");
		}else if (uri.find("/img")!=uri.npos){
			ui_server.send_binary_to_fd(fd,"./webui"+uri);
		}else{
			send_to_fd(fd,get_header("Connections"));
			send_to_fd(fd,get_connections_info());
			send_to_fd(fd,get_footer());
		}
		shutdown(fd,2);
		close(fd);
	}catch(...){
		error_log_no_msg("Error in ui_server.cpp: serve_server()");
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
//					close(client_sockfd);

				//If it isn’t the server, it must be client activity. If close is received, the client has gone away, and you remove it from the descriptor set. Otherwise, you “serve” the client as in the previous examples.
				}else{
					debug("else");
					ioctl(fd, FIONREAD, &nread);
					if(nread == 0) {
						debug("nothing to read");
						FD_CLR(fd, &ui_server.readfds);
						if FD_ISSET(fd, &ui_server.tuiclient_fds_set){
							FD_CLR(fd, &ui_server.tuiclient_fds_set);
						}
						close(fd);
						debug("Client parted from fd:"+toString(fd));
					}else{
						debug("reading buffer");
						char buffer[1000]="";
						if (nread!=read(fd, &buffer, nread)){
							debug("Not all data has been read from ui_client()");
						}
						string buffer_as_str=buffer;
						debug("received_from ui_client:"+buffer_as_str);
						if FD_ISSET(fd, &ui_server.tuiclient_fds_set){
							ui_server.serve_tuiclient(fd,buffer_as_str);
						}else{
							if (buffer_as_str.find("Host:")==buffer_as_str.npos){
								// it's tuiclient else it's browser
								FD_SET(fd, &ui_server.tuiclient_fds_set);
								ui_server.serve_tuiclient(fd,buffer_as_str);
							}else{  // it's browser
								ui_server.serve_browser(fd,buffer_as_str);
								FD_CLR(fd, &ui_server.readfds);
								close(fd);
							}
						}
					}
				}
			}
		}
	}
	return 0;
}
