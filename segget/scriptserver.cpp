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

#include "scriptserver.h"

map<std::string, Tvar_nums> variables_;

#define RESPONSE_OK 						"o<r>";
#define RESPONSE_UNKNOWN_VARIABLE			"u<r>";
#define RESPONSE_ERROR						"e<r>";

#define ACCEPT_SEGMENT						0;
#define ERROR_HAPPEND_SO_ACCEPT_SEGMENT		0;
#define REJECT_SEGMENT						1;

FILE * stderr_file;
FILE * stdout_file;
int script_server_sockfd, script_client_sockfd;

string set(uint connection_num, string var_name_and_value){
	try{
		string var_name, var_value;
		if (! split("<n>",var_name_and_value,var_name,var_value)){
			map<string,Tvar_nums>::const_iterator ci = variables_.find(var_name);
			if (ci == variables_.end()){
				return RESPONSE_UNKNOWN_VARIABLE;
			}else{
				switch (ci->second){
					case CONNECTION_MAX_SPEED_LIMIT:{
						ulong new_max_speed_limit=atol(var_value.c_str());
						if (new_max_speed_limit==0){
							error_log("Error in scriptserver.cpp: set("+var_name+"="
								+var_value+"): Can't convert "+var_value+" to integer");
							return RESPONSE_ERROR;
						}else{
							connection_array[connection_num].max_speed_limit=new_max_speed_limit;
							debug("set("+var_name+"="+var_value+")");
							return RESPONSE_OK;
						}
					}
					default:{
						error_log("Error in scriptserver.cpp: set("+var_name+"="
							+var_value+"): UNKNOWN VARIABLE");
						return RESPONSE_UNKNOWN_VARIABLE;
					}
				}
			}
		}
		return RESPONSE_ERROR;
	}catch(...){
		error_log("Error in scriptserver.cpp: set()");
		return RESPONSE_ERROR;
	}
}

string get(uint connection_num, string var_name){
	try{
		map<string,Tvar_nums>::const_iterator ci = variables_.find(var_name);
		if (ci == variables_.end()){
			return RESPONSE_UNKNOWN_VARIABLE;
		}else{
			switch (ci->second){
				case DISTFILE_NAME:{
					return connection_array[connection_num].segment->parent_distfile->name;
				}
				case DISTFILE_SIZE:{
					return toString(connection_array[connection_num].segment->parent_distfile->size);
				}
				case DISTFILE_DLD_SEGMENTS_COUNT:{
					return toString(connection_array[connection_num].segment->parent_distfile->dld_segments_count);
				}
				case DISTFILE_SEGMENTS_COUNT:{
					return toString(connection_array[connection_num].segment->parent_distfile->segments_count);
				}
				case DISTFILE_ACTIVE_CONNECTIONS_COUNT:{
					return toString(connection_array[connection_num].segment->parent_distfile->active_connections_num);
				}
				case SEGMENT_NUM:{
					return toString(connection_array[connection_num].segment->segment_num);
				}
				case SEGMENT_TRY_NUM:{
					return toString(connection_array[connection_num].segment->try_num);
				}
				case SEGMENT_SIZE:{
					return toString(connection_array[connection_num].segment->segment_size);
				}
				case SEGMENT_RANGE:{
					return connection_array[connection_num].segment->range;
				}
				case CONNECTION_NUM:{
					return toString(connection_num);
				}
				case CONNECTION_URL:{
					return connection_array[connection_num].url;
				}
				case NETWORK_NUM:{
					return toString(connection_array[connection_num].network_num);
				}
				case NETWORK_MODE:{
					return toString(network_array[connection_array[connection_num].network_num].network_mode);
				}
				case NETWORK_ACTIVE_CONNECTIONS_COUNT:{
					return toString(network_array[connection_array[connection_num].network_num].active_connections_num);
				}
				default: return RESPONSE_UNKNOWN_VARIABLE; //unknown variable
			}
		}
		return RESPONSE_ERROR;
	}catch(...){
		error_log("Error in scriptserver.cpp: get()");
		return RESPONSE_ERROR;
	}
}
void init_variables(){
	try{
		variables_["connection.num"]=CONNECTION_NUM;
		variables_["connection.url"]=CONNECTION_URL;
		variables_["connection.max_speed_limit"]=CONNECTION_MAX_SPEED_LIMIT;
		variables_["network.num"]=NETWORK_NUM;
		variables_["network.mode"]=NETWORK_MODE;
		variables_["network.active_connections_count"]=NETWORK_ACTIVE_CONNECTIONS_COUNT;
		variables_["distfile.name"]=DISTFILE_NAME;
		variables_["distfile.size"]=DISTFILE_SIZE;
		variables_["distfile.dld_segments_count"]=DISTFILE_DLD_SEGMENTS_COUNT;
		variables_["distfile.segments_count"]=DISTFILE_SEGMENTS_COUNT;
		variables_["distfile.active_connections_count"]=DISTFILE_ACTIVE_CONNECTIONS_COUNT;
		variables_["segment.num"]=SEGMENT_NUM;
		variables_["segment.try_num"]=SEGMENT_TRY_NUM;
		variables_["segment.size"]=SEGMENT_SIZE;
		variables_["segment.range"]=SEGMENT_RANGE;
//		variables_[""]=;
	}catch(...){
		error_log("Error in scriptserver.cpp: init_variables()");
	}
}
void send(int fd, string response){
	try{
		if (write(fd, response.c_str(), response.length())!=(int)response.length()){
			error_log("Error in scriptserver.cpp: send(): response msg size and sent data size are different.");
		};
	}catch(...){
		error_log("Error in scriptserver.cpp: send()");
	}
}

void killscript(int pID){
	try{
		debug("Before killing script");
/*		int  killReturn = kill( pID, SIGKILL);  // Kill child process
		if( killReturn == ESRCH){	// pid does not exist
		error_log("Python script does not exist!");
		}else if( killReturn == EPERM){ // No permission to send signal
		error_log("No permission to kill python script");
		}else debug("Signal to kill python script sent. All Ok!");
*/
		waitpid(pID, NULL, 0);
		close(script_server_sockfd);
		close(script_client_sockfd);
		debug("After killing script");
	}catch(...){
		error_log("Error in scriptserver.cpp: killscript()");
	}
}

bool run_user_python_script(uint connection_num){
	pid_t pID;
	try{
		init_variables();
		socklen_t server_len, client_len;
		struct sockaddr_un server_address;
		struct sockaddr_un client_address;

		int result;
		fd_set readfds, testfds;

		unlink("/tmp/segget_script_socket");
		// Create and name a socket for the server:
		script_server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
		server_address.sun_family = AF_UNIX;
		strcpy(server_address.sun_path, "/tmp/segget_script_socket");
		server_len = sizeof(server_address);
		bind(script_server_sockfd, (struct sockaddr *)&server_address, server_len);
		//Create a connection queue and initialize readfds to handle input from server_sockfd:
		listen(script_server_sockfd, 5);
		FD_ZERO(&readfds);
		FD_SET(script_server_sockfd, &readfds);

		pID = fork();
		if (pID == 0){ // child
			alarm(2); // 2 second limit for script to execute
			stderr_file = fopen((settings.logs_dir+"/net"
									+toString(connection_array[connection_num].network_num)
									+"_script_stderr.log").c_str(), "a+");
			if(stderr_file) {
				dup2(fileno(stderr_file), 2);
				fclose(stderr_file);
			}
			stdout_file = fopen((settings.logs_dir+"/net"
									+toString(connection_array[connection_num].network_num)
									+"_script_stdout.log").c_str(), "a+");
			if(stdout_file) {
				dup2(fileno(stdout_file), 1);
				fclose(stdout_file);
			}
			system((settings.python_path+" /home/mona/idfetcha/scripts/client.py").c_str());
			_exit(0);
		}else{
			if (pID < 0){            // failed to fork
				error_log("Error in scriptserver.cpp: failed to fork");
				return 0;
			}
		}

		error_log("Created pid:"+toString(pID));
		// parent
		//Now wait for clients and requests. Because you have passed a null pointer as the timeout parameter, no timeout will occur. The program will exit and report an error if select returns a value less than 1:
		struct timeval user_script_start_time;
		gettimeofday(&user_script_start_time,NULL);
		
		while(1000>time_left_from(user_script_start_time)) {
			int fd;
			int nread;
			testfds = readfds;

			debug("scriptserver is waiting for connections");
			struct timeval timeout;
			timeout.tv_sec = 1;
			timeout.tv_usec = 0;

			result = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, &timeout);
			if(result < 1) {
				error_log("Error in scriptserver.cpp: run_script_server(): error on select ");
				killscript(pID);
				return ERROR_HAPPEND_SO_ACCEPT_SEGMENT;
			}
			//Once you know you’ve got activity, you can find which descriptor it’s on by checking each in turn using FD_ISSET:
			for(fd = 0; fd < FD_SETSIZE; fd++) {
				if(FD_ISSET(fd,&testfds)) {
					//If the activity is on server_sockfd, it must be a request for a new connection, and you add the associated client_sockfd to the descriptor set:
					if (fd==script_server_sockfd){
							debug("new script client - read");
							client_len = sizeof(client_address);
							script_client_sockfd = accept(script_server_sockfd,
								(struct sockaddr *)&client_address, &client_len);
							FD_SET(script_client_sockfd, &readfds);
							debug("adding script client on fd:"+toString(script_client_sockfd));
							break;
					}else{
						script_client_sockfd=fd;
						ioctl(fd, FIONREAD, &nread);
						if(nread == 0) {
							debug("removing script client from fd:"+toString(fd));
						}else{
							char buffer[100000]="";
							if (nread!=read(fd, &buffer, nread)){
								error_log("Error in scriptserver.cpp : run_script_server(): Not all data has been read from script-client");
							}
							string recv_msg=noupper(buffer);
							debug("SCRIPT-SERVER RECIVED:"+recv_msg);
							string command, arguments, send_response;
							if (! split("<c>",recv_msg,command,arguments)){
								switch (command[0]){
									case 'g':{
										debug("getting: "+arguments);
										send(fd,get(connection_num,arguments));
										break;
									}
									case 's':{
										debug("setting: "+arguments);
										send(fd,set(connection_num,arguments));
										break;
									}
									case 'a':{
										debug("accepting segment: ");
										killscript(pID);
										return ACCEPT_SEGMENT;
									}
									case 'r':{
										debug("rejecting segment: ");
										killscript(pID);
										return REJECT_SEGMENT;
									}
								}
							}
						}
					}
				}
			}
		}
		killscript(pID);
		return ACCEPT_SEGMENT;
	}catch(...){
		error_log("Error in scriptserver.cpp: run_script_server()");
		return ERROR_HAPPEND_SO_ACCEPT_SEGMENT;
	}
	killscript(pID);
}