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

#include "request.h"

int request(string msg){
	try{
		int len;
		struct sockaddr_in address;
		int result;
		//Create a socket for the client:
		sockfd = socket(AF_INET, SOCK_STREAM, 0);

		//Name the socket, as agreed with the server:
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = inet_addr(settings.request_ip.c_str());
		address.sin_port = htons(settings.request_port);
		len = sizeof(address);

		//Connect your socket to the server’s socket:
		result = connect(sockfd, (struct sockaddr *)&address, len);
		if(result == -1) {
			printout("Can't connect to segget daemon");
			return R_PF_ERROR_ADDING_TO_PROXY_QUEUE;
		}
		if (msg.length()>90000){return R_PF_ERROR_ADDING_TO_PROXY_QUEUE;};
		char send_buffer[100000];

		strcpy(send_buffer,msg.c_str());
		//You can now read and write via sockfd:
		if (write(sockfd, send_buffer, strlen(send_buffer))!=(int)msg.length()){
			printout("Error in request.cpp: request(): request msg size and sent data size are different.");
		};

		fd_set readfds, testfds;
		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);
		testfds = readfds;

		struct timeval response_timeout;
		response_timeout.tv_sec=1;
		response_timeout.tv_usec=0;

		result = select(FD_SETSIZE, &testfds, (fd_set *)0,
		(fd_set *)0, &response_timeout);

		if(FD_ISSET(sockfd,&testfds)) {
			int nread;
			ioctl(sockfd, FIONREAD, &nread);

			char recv_buffer[1000];
			if(nread == 0) {
				close(sockfd);
				printout("Error in request.cpp: request(): no response from segget daemon");
			}else{
				if (nread!=read(sockfd, recv_buffer, nread)){
					printout("Error in request.cpp: request(): response msg size and received data size are different.");
				};
				return decode_server_response(recv_buffer);
			}
		}else{
			printout("Error in request.cpp: request(): zero size response from segget daemon");
		}
		close(sockfd);
		return R_PF_ERROR_ADDING_TO_PROXY_QUEUE;
	}catch(...){
		printout("Error: request.cpp: request()");
		return R_PF_ERROR_ADDING_TO_PROXY_QUEUE;
	}
}

int load_pkgs(){
	try{
		ifstream json_pkg_list_file;
		json_pkg_list_file.exceptions (ofstream::failbit | ofstream::badbit);
		try{
			json_pkg_list_file.open(settings.pkg_list_file_name.c_str());
		}catch(...){
			printout("Error: Can't open distfile list file: "+settings.pkg_list_file_name);
			return 1;
		}
		string buff((std::istreambuf_iterator<char>(json_pkg_list_file)), std::istreambuf_iterator<char>());
		try{
			json_pkg_list_file>>buff;
		}catch(std::ifstream::failure e){
			if (!json_pkg_list_file.eof()){
				printout("Error: Can't read pkg.list file: "+(string)e.what());
				return 1;
			}
		}catch(...){
			printout("Unknown Error: Can't read pkg.list file");
			return 1;
		}
		try{
			json_pkg_list_file.close();
		}catch(...){
			printout("Error: Can't close pkg.list file");
		}
		try{
			json_object *json_array_pkg_list;
			json_array_pkg_list = json_tokener_parse(buff.c_str());
			if (is_error(json_array_pkg_list))
				printout("error parsing json:"+toString(json_tokener_errors[-(unsigned long)json_array_pkg_list]));
			else {
				uint pkg_count=json_object_array_length(json_array_pkg_list);
				// create 0 pkg for distfiles to provide proxy-fetcher
				for(uint array_item_num=0;array_item_num<pkg_count;array_item_num++){
					json_object* json_obj_pkg=json_object_array_get_idx(json_array_pkg_list,array_item_num);
					json_object* json_array_distfile_list=json_object_object_get(json_obj_pkg,"distfile_list");
					ulong distfile_counter=json_object_array_length(json_array_distfile_list);
					for(uint distfile_array_item_num=0;distfile_array_item_num<distfile_counter;distfile_array_item_num++){
						json_object* json_obj_distfile=json_object_array_get_idx(json_array_distfile_list,distfile_array_item_num);
						request(json_object_to_json_string(json_obj_distfile));
						cout<< json_object_to_json_string(json_object_object_get(json_obj_distfile,"name"))<< endl;
					}
				}
			}
		}catch(...){
			printout("Error: Can't parse json data from pkg.list file");
			return 1;
		}
		if (settings.keep_pkg_list_file){
			printout(settings.pkg_list_file_name+" will NOT be deleted");
		}else{
			if(remove(settings.pkg_list_file_name.c_str()) != 0 )
				printout("Error in request.cpp: load_pkgs(): can't delete:"+settings.pkg_list_file_name);
			else
				printout(settings.pkg_list_file_name+" deleted" );
		}
		return 0;
	}catch(...){
		printout("Error in request.cpp: load_pkgs()");
		return 1;
	}
}

void _exit(int sig){
	try{
		close(sockfd);
	}
	catch(...)
	{
		//error while clossing server_sockfd
	}
	cout<<"Exiting. (Cought signal:"+toString(sig)+")\n\n";
	exit(0);
}

int routine(){
	try{
		signal(SIGABRT,_exit);//If program aborts go to assigned function "segget_exit".
		signal(SIGTERM,_exit);//If program terminates go to assigned function "segget_exit".
		signal(SIGINT,_exit);//If program terminates go to assigned function "segget_exit".
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
		}catch(...){
			//error while loading pkgs
		}
		return 0;
	}catch(...)
	{
		printout("Error in request.cpp: routine()");
	}
	return 1;
}

void show_help(){
	try{
		printout("Usage: request [OPTIONS]");
		printout("Options:");
		printout("--conf-dir=PATH_TO_CONFIG_DIR           Specify path to a dir containing segget.conf file. By default it's /etc/seggetd");
		printout("--help                                  Show this help.");
		printout("--keep-pkglist-file                     Do NOT delete pkg.list file after sending request to segget daemon.");
		printout("--pkglist-file=PATH_TO_PKGLIST_FILE     Specify path to pkg.list file. By default it's /var/tmp/seggetd/pkg.list");
		printout("");
	}catch(...){
		printout("Error in request.cpp: show_help()");
	}
}

int parse_cli_arguments(int argc, char* argv[]){
	try{
		string option,name,value;
		int posEqual;
		for(int i = 0; i < argc; i++){
			option=argv[i];
			posEqual=option.find('=');
			name  = trim(option.substr(0,posEqual));
			value = trim(option.substr(posEqual+1));
			if (name=="--conf-dir") {settings.conf_dir=value; continue;};
			if (name=="--pkglist-file") {settings.pkg_list_file_name=value; continue;};
			if (name=="--keep-pkglist-file") {settings.keep_pkg_list_file=true; continue;};
			if (name=="--help") {show_help(); _exit(0);};
		}
		return 0;
	}catch(...){
		printout("Error in request.cpp: init_curses()");
	}
	return 1;
}

int main(int argc, char* argv[])
{
	try{
		parse_cli_arguments(argc, argv);
		routine();
		exit (0);
	}catch(...){
		printout("Error in request.cpp main()");
	}
	exit(1);
}