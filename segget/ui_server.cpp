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
		uint distfile_percent;
		if (a_distfile->size>0){
			distfile_percent=a_distfile->dld_bytes*100/a_distfile->size;
		}else{
			distfile_percent=50;
		}
		string result=(string)"<td align=\"center\"><table width=\"100\" border=\"0\" cellpadding=\"0\" frame=\"void\"><tr>"
				+((distfile_percent>0)?"<td><img src=\"/img/blue_progress.jpg\" height=\"20\" width=\""+toString(distfile_percent)+"\"/></td>"
					:"")
				+((100-distfile_percent>0)?"<td><img src=\"/img/bw_progress.jpg\" height=\"20\" width=\""+toString(100-distfile_percent)+"\" /></td>"
					:"")
				+"</tr></table>"
				+toString(distfile_percent)+"%"
				+"</td><td>"+a_distfile->name
				+"</td><td align=\"center\" bgcolor=\""+a_distfile->statusToColor()+"\">"+a_distfile->statusToString()
				+"</td><td align=\"right\">"+toString(a_distfile->dld_segments_count)
				+"</td><td align=\"right\">"+toString(a_distfile->segments_count)
				+"</td><td align=\"right\">"+toString(a_distfile->dld_bytes)
				+"</td><td align=\"right\">"+toString(a_distfile->size)
				+"</td>";
		return result;
	}catch(...){
		error_log("Error: ui_server.cpp: serve_browser_distfile_progress()");
		return "";
	}
}

string Tui_server::get_html_header(string title){
	string header;
	header=(string)"HTTP/1.0 200 OK\n\n"
		+"<html><head>"
//		+"<meta http-equiv=\"refresh\" content=\"1\" >"
		+"<title> Segget - "+title+"</title></head>"
		+"<body>"
		+"<table border=\"0\">\n"
			+"<tr align=\"center\">"
				+"<td><a href=\"connections\"><img src=\"/img/connections.jpg\" alt=\"Segments\" height=50 width=50/></a></td>"
				+"<td><a href=\"distfiles\"><img src=\"/img/distfiles.png\" alt=\"Distfiles\" height=50 width=50/></a></td>"
				+"<td><a href=\"stats\"><img src=\"/img/stats.jpg\" alt=\"Stats\" height=50 width=50/></a></td>"
				+"<td><a href=\"mirrors_stats\"><img src=\"/img/mirrors.jpg\" alt=\"mirrors\" height=50 width=50/></a></td>"
				+"<td><a href=\"log\"><img src=\"/img/log.png\" alt=\"Log\" height=50 width=50/></a></td>"
				+"<td><a href=\"errors_log\"><img src=\"/img/errors_log.jpg\" alt=\"Errors log\" height=50 width=50/></a></td>"
				+"<td><a href=\"rss\"><img src=\"/img/rss.jpg\" alt=\"Log\" height=50 width=50/></a></td>"
			+"</tr>\n"
			+"<tr align=\"center\">"
				+"<td><a href=\"connections\">Connections</a></td>"
				+"<td><a href=\"distfiles\">Distfiles</a></td>"
				+"<td><a href=\"stats\">Stats</a></td>"
				+"<td><a href=\"mirrors_stats\">Mirrors</a></td>"
				+"<td><a href=\"log\">Log</a></td>"
				+"<td><a href=\"errors_log\">Errors log</a></td>"
				+"<td><a href=\"rss\">RSS</a></td>"
		+"</tr>\n"
		+"</table>\n"
		+"<h1>"+title+"</h1>\n";
	return header;
}

string Tui_server::get_html_footer(){
	return "</body></html>";
}

string Tui_server::get_html_stats(){
	ulong total_progress;
	if (stats.total_size>0){
		total_progress=(stats.total_size-stats.dld_size)*100/stats.total_size;
	}else total_progress=0;
	string stats_str=
		(string)"<table border=\"1\">\n"
			+"<tr>\n"
			+"	<td>Up time</td>\n"
			+"	<td align=\"right\">"+secsToString(time_left_since(stats.segget_start_time)/1000)+"</td>\n"
			+"</tr>\n"
			+"<tr>\n"
			+"	<th colspan=\"2\">Distfiles</th>\n"
			+"</tr>\n"
				+"<tr><td>Downloaded distfiles</td><td align=\"right\">"+toString(stats.dld_distfiles_count)+"</td></tr>\n"
				+"<tr><td>Bytes downloaded</td><td align=\"right\">"+toString(stats.dld_size)+"</td></tr>\n"
				+"<tr><td>Total progress</td><td align=\"right\">"+toString(total_progress)+"%</td></tr>\n"
				+"<tr><td>Distfiles left to download</td><td align=\"right\">"+toString(stats.distfiles_count-stats.dld_distfiles_count)+"</td></tr>\n"
				+"<tr><td>Bytes left to download</td><td align=\"right\">"+toString(stats.total_size-stats.dld_size)+"</td></tr>\n"
				+"<tr><td>Distfiles total</td><td align=\"right\">"+toString(stats.distfiles_count)+"</td></tr>\n"
				+"<tr><td>Bytes total</td><td align=\"right\">"+toString(stats.total_size)+"</td></tr>\n"
			+"<tr><th colspan=\"2\">Segments</th></tr>\n"
				+"<tr><td>Downloaded segments</td><td align=\"right\">"+toString(stats.dld_segments_count)+"</td></tr>\n"
				+"<tr><td>Segmentss left to download</td><td align=\"right\">"+toString(stats.segments_count-stats.dld_segments_count)+"</td></tr>\n"
				+"<tr><td>Segments total</td><td align=\"right\">"+toString(stats.segments_count)+"</td></tr>\n"
			+"<tr><th colspan=\"2\">Connections</th></tr>\n"
				+"<tr><td>Failed connections</td><td align=\"right\">"+toString(stats.fails_counter)+"</td></tr>\n"
				+"<tr><td>AVG speed of active connections</td><td align=\"right\">"+speedToString(stats.avg_total_speed)+"</td></tr>\n"
//				+"<tr><td>AVG speed since segget start</td><td align=\"right\">"+speedToString(stats.dld_size*1000/time_left_since(stats.segget_start_time))+"</td></tr>"
//				+"<tr><td>AVG speed since segget start except idle times</td><td align=\"right\">"+speedToString(stats.avg_total_speed)+"</td></tr>"
		+"</table>\n";
	return stats_str;
}


string Tui_server::get_html_mirrors_stats(){
	string stats_str=(string)"<table border=\"1\" width=\"100%\">\n"
			+"<tr>\n<th>URL</th>\n"
				+"<th>Active connections</th>\n"
				+"<th>AVG speed per connection**</th>\n"
				+"<th>Failed downloads</th>\n"
				+"<th>Successful downloads</th>\n"
				+"<th>Fail ratio</th>\n"
				+"<th>Usage time**, sec</th>\n"
				+"<th>Succesfuly downloaded bytes</th>\n"
				+"<th>Honesty</th>\n"
			+"</tr>\n";
	map<string,Tmirror *>::iterator iter; 
	for( iter = mirror_list.begin(); iter != mirror_list.end(); iter++ ) {
		ulong avg_speed;
		if ((iter->second->dld_size!=1) and (iter->second->dld_time>0)){
			avg_speed=(iter->second->dld_size)/iter->second->dld_time;
		}else avg_speed=0;

		ulong fail_ratio;
		if (iter->second->failed_downloads+iter->second->successful_downloads>0){
			fail_ratio=(iter->second->failed_downloads)*100/(iter->second->failed_downloads+iter->second->successful_downloads);
		}else fail_ratio=0;
		stats_str=stats_str
			+"<tr>\n<td>\n"+iter->first+"</td>\n"
				+"<td align=\"right\">"+toString(iter->second->active_num)+"</td>\n"
				+"<td align=\"right\">"+speedToString(avg_speed)+"</td>\n"
				+"<td align=\"right\">"+toString(iter->second->failed_downloads)+"</td>\n"
				+"<td align=\"right\">"+toString(iter->second->successful_downloads)+"</td>\n"
				+"<td align=\"right\">"+toString(fail_ratio)+"%</td>\n"
				+"<td align=\"right\">"+toString(iter->second->dld_time)+"</td>\n"
				+"<td align=\"right\">"+toString(iter->second->dld_size)+"</td>\n"
				+"<td align=\"right\" width=\"15%\">"+toString(iter->second->honesty)+"</td>\n"
			+"</tr>\n";
	}
	stats_str=stats_str+"</table>\n";

	stats_str=stats_str+"<br><p>** NOTE: When a mirror has N simultaneous connections "
		+"\"Usage time\" will be increasing N times faster, therefore \"AVG speed per connection\" will "
		+"be smaller than average outgoing rate provided by this mirror to your host "
		+"(which is a summary speed of the simultaneous connections to this mirror)</p>";
	return stats_str;
}


string Tui_server::get_html_connections(){
	try{
		string result=(string)"<center>\n"
			+"<h3>Active connections: "+toString(stats.active_connections_counter)+"/"+toString(settings.max_connections)+"</h3>\n"
		+"<table border=\"1\" width=\"100%\">\n"
			+"<tr>\n"
				+"<th rowspan=\"2\" width=\"110\">Distfile progress</th>\n"
				+"<th rowspan=\"2\">Distfile name</th>\n"
				+"<th rowspan=\"2\" width=\"110\">Segment progress</th>\n"
				+"<th rowspan=\"2\">Segment #</th>\n"
				+"<th rowspan=\"2\">Try</th>\n"
				+"<th colspan=\"2\">Network</th>\n"
				+"<th colspan=\"2\">Bytes</th>\n"
				+"<th colspan=\"2\">Speed</th>\n"
				+"<th rowspan=\"2\" width=\"100\">ETA</th>\n"
			+"</tr>\n"
			+"<tr>\n"
				+"<th>Num</th>\n"
				+"<th>Type</th>\n"
				+"<th>Downloaded</th>\n"
				+"<th>Total</th>\n"
				+"<th width=\"100\">Current</th>\n"
				+"<th width=\"100\">Average</th>\n"
			+"</tr>\n";
		for (uint connection_num = 0; connection_num < settings.max_connections; ++connection_num) {
//			debug("connection_num:"+toString(connection_num));
			if (connection_array[connection_num].active){
				result=result+"<tr>"+connection_array[connection_num].get_html_connection_progress()+"</tr>\n";
			}
		}
		result=result+"</table>\n</center>\n";
		return result;
	}catch(...){
		error_log("Error: ui_server.cpp: get_connections_info()");
		return "";
	}
}
string Tui_server::get_html_distfiles(){
	try{
		debug("Sending to client distfiles_num:"+toString(request_server_pkg.Pdistfile_list.size()));
		string distfiles_html=(string)"<center><table border=\"1\" width=\"100%\">\n"
				+"<tr>"
				+"	<th rowspan=\"2\" width=\"110\">Progress"
				+"	</th><th rowspan=\"2\">Name"
				+"	</th><th rowspan=\"2\">Status"
				+"	</th><th colspan=\"2\">Segments"
				+"	</th><th colspan=\"2\">Bytes"
				+"	</th>"
				+"</tr>\n<tr>"
				+"	<th>Downloaded"
				+"	</th><th>Total"
				+"	</th><th>Downloaded"
				+"	</th><th>Total"
				+"	</th></tr>\n";
				for (ulong distfile_num=0; distfile_num<request_server_pkg.Pdistfile_list.size(); distfile_num++){
					distfiles_html=distfiles_html+"<tr>"+serve_browser_distfile_progress(request_server_pkg.Pdistfile_list[distfile_num])+"</tr>\n";
					debug("Sending to client:"+request_server_pkg.Pdistfile_list[distfile_num]->get_distfile_progress_str());
				}
				for (ulong distfile_num=0; distfile_num<proxy_fetcher_pkg.Pdistfile_list.size(); distfile_num++){
					distfiles_html=distfiles_html+"<tr>"+serve_browser_distfile_progress(proxy_fetcher_pkg.Pdistfile_list[distfile_num])+"</tr>\n";
					debug("Sending to client:"+proxy_fetcher_pkg.Pdistfile_list[distfile_num]->get_distfile_progress_str());
				}
			distfiles_html=distfiles_html+"</table></center>";
		return distfiles_html;
	}catch(...){
		error_log("Error: ui_server.cpp: get_connections_info()");
		return "";
	}
}

string Tui_server::get_html_log(){
	try{
		string log_html=(string)"<center><table border=\"1\" width=\"100%\">";
			for (uint log_line_num=0; log_line_num<log_lines.size(); log_line_num++){
				log_html=log_html+"<tr><td>"+log_lines[log_line_num]+"</td></tr>";
			}
			log_html=log_html+"</table></center>";
		return log_html;
	}catch(...){
		error_log("Error: ui_server.cpp: get_html_log()");
		return "";
	}
}

string Tui_server::get_html_errors_log(){
	try{
		debug("Sending to client distfiles_num:"+toString(request_server_pkg.Pdistfile_list.size()));
		string error_log_html=(string)"<center><table border=\"1\" width=\"100%\">";
		for (uint error_log_line_num=0; error_log_line_num<error_log_lines.size(); error_log_line_num++){
			error_log_html=error_log_html+"<tr><td>"+error_log_lines[error_log_line_num]+"</td></tr>";
		}
		error_log_html=error_log_html+"</table></center>";
		return error_log_html;
	}catch(...){
		error_log("Error: ui_server.cpp: get_html_error_log()");
		return "";
	}
}

string Tui_server::get_rss_info(){
	try{
			string rss_result=
				(string)"<?xml version=\"1.0\" encoding=\"UTF-8\"?><rss version=\"2.0\"> "
				"<channel>"
					+"<title>"+settings.rss_title+"</title>"
					+"<description>"+settings.rss_description+"</description>"
					+"<link>"+settings.provide_mirror_to_others_url+"</link>"
					+"<pubDate>"+get_time(settings.general_log_time_format)+"</pubDate>"
					+"<ttl>5</ttl>"
					+"<image>"
						+"<title>"+"settings.rss_title"+"</title>"
						+"<url>/img/segget_feed.jpg</url>"
						+"<link>http://simsim/forum/index.php</link>"
					+"</image>";
			for (uint rss_line_num=0; rss_line_num<rss_distfile_lines.size(); rss_line_num++){
				rss_result=rss_result+
					"<item>"
						+"<title>"+rss_distfile_lines[rss_line_num]+"</title>"
						+"<link><![CDATA["+settings.provide_mirror_to_others_url+"/"+rss_distfile_lines[rss_line_num]+"]]></link> "
						+"<starter>"+"settings.rss_starter"+"</starter>"
						+"<author>seggetd</author>"
						+"<description><![CDATA[Downloaded distfile "+rss_distfile_lines[rss_line_num]
						+" (size: "+toString(rss_size_lines[rss_line_num])+" B)]]></description>"
						+"<pubDate>"+rss_time_lines[rss_line_num]+"</pubDate>"
						+"<guid isPermaLink=\"false\">"+toString(rss_line_num)+"</guid>"
					+"</item>";
			}
			rss_result=rss_result+"</channel></rss>";
		return rss_result;
	}catch(...){
		error_log("Error: ui_server.cpp: get_rss_info()");
		return "";
	}
}
string Tui_server::get_ajax_for(string content_name){
			string ajax_str=(string)"\n<script  type=\"text/javascript\">\n"
				+"	var xmlDoc;"
				+"	function process(){\n"
				+"		if ( xmlDoc.readyState != 4 ) return;\n"
				+"		document.getElementById(\""+content_name+"\").innerHTML = xmlDoc.responseText;\n"
				+"	}\n"
				+"	function load() {"
				+"		if (window.XMLHttpRequest) {\n" // Mozilla, Safari, ...
				+"			xmlDoc = new XMLHttpRequest();\n"
				+"			xmlDoc.onload = process ;\n"
				+"		} else if (window.ActiveXObject) {\n" // IE
				+"			xmlDoc = new ActiveXObject(\"Microsoft.XMLHTTP\");\n"
				+"			xmlDoc.onreadystatechange = process ;\n"
				+"		}else{\n"
				+"			return false;\n"
				+"		}\n"
				+"		xmlDoc.abort();\n"
				+"		xmlDoc.open(\"GET\",\""+content_name+"_"+"\",true);\n"
				+"		xmlDoc.send(null);\n"
				+"	}\n"
				+"	function refresh(){\n"
				+"		if ( xmlDoc.readyState != 4 ){\n"
				+"			document.getElementById(\""+content_name+"\").innerHTML=\"<h2 color='#FF0000'>Disconnected</h2>\"\n"
				+"		};\n"
				+"		xmlDoc.abort();\n"
				+"		load();\n"
				+"		setTimeout(\"refresh()\", 1000);\n"
				+"	};\n"
				+"	load();\n"
				+"	setTimeout(\"refresh()\", 1000);\n"
				+"</script>\n";
	return ajax_str;
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
			send_to_fd(fd,get_html_header("Connections"));
			send_to_fd(fd,"<span id=\"connections\">\n");
			send_to_fd(fd,get_html_connections());
			send_to_fd(fd,"</span>\n");
			send_to_fd(fd,get_ajax_for("connections"));
			send_to_fd(fd,get_html_footer());
		}else if (uri=="/connections_"){
			send_to_fd(fd,get_html_connections());
		}else if ((uri=="/rss") or (uri=="/rss.rss") or (uri=="/rss.xml")){
			send_to_fd(fd,get_rss_info());
		}else if (uri=="/stats"){
			send_to_fd(fd,get_html_header("Stats"));
			send_to_fd(fd,"<span id=\"stats\">\n");
			send_to_fd(fd,get_html_stats());
			send_to_fd(fd,"</span>\n");
			send_to_fd(fd,get_ajax_for("stats"));
			send_to_fd(fd,get_html_footer());
		}else if (uri=="/stats_"){
			ui_server.send_to_fd(fd,get_html_stats());
		}else if (uri=="/mirrors_stats"){
			send_to_fd(fd,get_html_header("Mirrors stats"));
			send_to_fd(fd,"<span id=\"mirrors_stats\">\n");
			send_to_fd(fd,get_html_mirrors_stats());
			send_to_fd(fd,"</span>\n");
			send_to_fd(fd,get_ajax_for("mirrors_stats"));
			send_to_fd(fd,get_html_footer());
		}else if (uri=="/mirrors_stats_"){
			send_to_fd(fd,get_html_mirrors_stats());
		}else if (uri=="/log"){
			send_to_fd(fd,get_html_header("Log"));
			send_to_fd(fd,"<span id=\"log\">\n");
			send_to_fd(fd,get_html_log());
			send_to_fd(fd,"</span>\n");
			send_to_fd(fd,get_ajax_for("log"));
			send_to_fd(fd,get_html_footer());
		}else if (uri=="/log_"){
			send_to_fd(fd,get_html_log());
		}else if (uri=="/errors_log"){
			send_to_fd(fd,get_html_header("Errors log"));
			send_to_fd(fd,"<span id=\"errors_log\">\n");
			send_to_fd(fd,get_html_errors_log());
			send_to_fd(fd,"</span>\n");
			send_to_fd(fd,get_ajax_for("errors_log"));
			send_to_fd(fd,get_html_footer());
		}else if (uri=="/errors_log_"){
			send_to_fd(fd,get_html_errors_log());
		}else if (uri=="/distfiles"){
			send_to_fd(fd,get_html_header("Distfiles"));
			send_to_fd(fd,"<span id=\"distfiles\">\n");
			send_to_fd(fd,get_html_distfiles());
			send_to_fd(fd,"</span>\n");
			send_to_fd(fd,get_ajax_for("distfiles"));
			send_to_fd(fd,get_html_footer());
		}else if (uri=="/distfiles_"){
			ui_server.send_to_fd(fd,get_html_distfiles());
		}else if (uri=="/favicon.ico"){
			ui_server.send_binary_to_fd(fd,"./webui/img/favicon.ico");
		}else if (uri.find("/img")!=uri.npos){
			ui_server.send_binary_to_fd(fd,"./webui"+uri);
		}else{
			send_to_fd(fd,get_html_header("Connections"));
			send_to_fd(fd,"<span id=\"connections\">\n");
			send_to_fd(fd,get_html_connections());
			send_to_fd(fd,"</span>\n");
			send_to_fd(fd,get_ajax_for("connections"));
			send_to_fd(fd,get_html_footer());
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
