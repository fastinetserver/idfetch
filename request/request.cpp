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
//	gettimeofday(&network_distfile_brokers_array[network_num].last_request_time, NULL);
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
		merror("Can't connect to segget daemon");
		return R_PF_ERROR_ADDING_TO_PROXY_QUEUE;
	}
	if (msg.length()>90000){return R_PF_ERROR_ADDING_TO_PROXY_QUEUE;};
	char send_buffer[100000];
	//	char recv_buffer[256];
	strcpy(send_buffer,msg.c_str());
	//You can now read and write via sockfd:
	if (write(sockfd, send_buffer, strlen(send_buffer))!=(int)msg.length()){
		merror("Error in request.cpp: request(): request msg size and sent data size are different.");
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
			merror("Error in request.cpp: request(): no response from segget daemon");
		}else{
			if (nread!=read(sockfd, recv_buffer, nread)){
				merror("Error in request.cpp: request(): response msg size and received data size are different.");
			};
			return decode_server_response(recv_buffer);
		}
	}else{
		merror("Error in request.cpp: request(): zero size response from segget daemon");
	}
	close(sockfd);
	return R_PF_ERROR_ADDING_TO_PROXY_QUEUE;
}

int load_pkgs(){
	try{
		ifstream json_pkg_list_file;
		json_pkg_list_file.exceptions (ofstream::failbit | ofstream::badbit);
		try{
			json_pkg_list_file.open(settings.pkg_list_file.c_str());
		}catch(...){
			merror("Error: Can't open distfile list file: "+settings.pkg_list_file);
			return 1;
		}
		string buff((std::istreambuf_iterator<char>(json_pkg_list_file)), std::istreambuf_iterator<char>());
		try{
			json_pkg_list_file>>buff;
		}catch(std::ifstream::failure e){
			if (!json_pkg_list_file.eof()){
				merror("Error: Can't read pkg.list file: "+(string)e.what());
				return 1;
			}
		}catch(...){
			perror("Unknown Error: Can't read pkg.list file");
			return 1;
		}
		try{
			json_pkg_list_file.close();
		}catch(...){
			perror("Error: Can't close pkg.list file");
		}
		try{
			json_object *json_array_pkg_list;
			json_array_pkg_list = json_tokener_parse(buff.c_str());
			if (is_error(json_array_pkg_list))
				merror("error parsing json:"+toString(json_tokener_errors[-(unsigned long)json_array_pkg_list]));
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
			return 0;
		}catch(...){
			perror("Error: Can't parse json data from pkg.list file");
			return 1;
		}
	}catch(...){
		perror("Error in segget.cpp: load_pkgs()");
		return 1;
	}
}
/*
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
*/

void segget_exit(int sig){
	try{
//		for(uint fd = 0; fd <= ui_server.max_fd_num; fd++) {
//			close(fd);
//		}
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
		signal(SIGABRT,segget_exit);//If program aborts go to assigned function "segget_exit".
		signal(SIGTERM,segget_exit);//If program terminates go to assigned function "segget_exit".
		signal(SIGINT,segget_exit);//If program terminates go to assigned function "segget_exit".
//		prev_time=time((time_t *)NULL);
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
		try{
			//show_pkgs();
//			stats.show_totals();
		}catch(...){
			//error while showing stats
		}
		try{
//			download_pkgs();
		}catch(...){
			//error while downloading_pkgs
		}
		return 0;
	}catch(...)
	{
		perror("Error in segget.cpp: routine()");
	}
	return 1;
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
			if (name=="--conf-dir") {settings.conf_dir=value; continue;};
			if (name=="--pkglist-file") {settings.pkg_list_file=value; continue;};
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
		routine();
		exit (0);
	}catch(...){
		perror("Error in segget.cpp main()");
	}
	exit(1);
}