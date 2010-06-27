#ifndef __SEGMENT_H__
#define __SEGMENT_H__
#include <sstream> 
#include <fstream>
#include <cstring>
#include <string>
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <ncurses.h>
#include <curl/curl.h>
#include "settings.h"
#include "stats.cpp"

using namespace std;

extern Tsettings settings;
unsigned long downloaded_bytes=0;
size_t write_data(void *buffer, size_t size, size_t nmemb, void *cur_segment);

enum Tstatus{WAITING, DOWNLOADING, DOWNLOADED, FAILED};

class Tsegment{
	private:
		CURL *easyhandle;
		char* urllist;
	public:
		string file_name;
		Tstatus status;
		uint try_num;
		void* parent_distfile;
		uint connection_num;
		uint segment_num;
		uint segment_size;
		unsigned long downloaded_bytes;
		string url;
		string range;
		ofstream segment_file;
		Tsegment():
			easyhandle(0),
			urllist(0),
			file_name(""),
			status(WAITING),
			try_num(0),
			parent_distfile(0),
			connection_num(0),
			segment_num(0),
			segment_size(1000),
			downloaded_bytes(0),
			url(""),
			range(""),
			segment_file(0)
			{};
		Tsegment(const Tsegment &L);             // copy constructor
		Tsegment & operator=(const Tsegment &L);
		~Tsegment();
		void set_segment(void *prnt_distfile, uint seg_num, string distfile_name, ulong default_seg_size, ulong range_end);
		void prepare_for_connection(CURLM *cm, uint con_num, uint distfile_num, string segment_url);
		string get_file_name(){return file_name;};
		int add_easy_handle_to_multi(CURLM *cm);
};

Tsegment *segments_in_progress[MAX_CONNECTS]={0};

void Tsegment::set_segment(void *prnt_distfile, uint seg_num, string distfile_name, ulong default_seg_size, ulong range_end){
	try{
		parent_distfile=prnt_distfile;
		segment_num=seg_num;
		segment_size=range_end-seg_num*default_seg_size+1;
		range=toString(seg_num*default_seg_size)+"-"+toString(range_end);
		file_name="."+distfile_name+".seg"+toString(seg_num);
		ulong downloaded_size;
		if (settings.get_resume()){
			//check if downloaded
			ifstream file((settings.segments_dir+"/"+file_name).c_str(), ios::in|ios::binary);
			ulong start = file.tellg();
			file.seekg (0, ios::end);
			ulong end = file.tellg();
			downloaded_size = end - start;
			debug("seg:"+toString(seg_num)+" Dsize="+toString(downloaded_size)+" seg_size="+toString(segment_size));
			file.close();
			if (downloaded_size==segment_size){
				status=DOWNLOADED;
				debug("seg:"+toString(seg_num)+" Downloaded");
			}
			else{
				debug("seg:"+toString(seg_num)+" not downloaded");
				downloaded_bytes=0;
			}
		}
	}catch(...){
		error_log("Error in segment.cpp: prepare_for_connection()");
	}
}
void Tsegment::prepare_for_connection(CURLM *cm, uint con_num, uint distfile_num, string segment_url){
	try{
		msg_connecting(con_num,distfile_num, segment_num,"Downloading from "+segment_url);
		segments_in_progress[con_num]=this;
		status=DOWNLOADING;
		downloaded_bytes=0;
		connection_num=con_num;
		connection_array[con_num].start_time=time((time_t *)NULL);
		url=segment_url;
		try_num++;
		add_easy_handle_to_multi(cm);
	}catch(...){
		error_log("Error in segment.cpp: prepare_for_connection()");
	}
}

Tsegment::~Tsegment(){
	//try
		segment_file.close();
}

int Tsegment::add_easy_handle_to_multi(CURLM *cm){
	try{
		segment_file.open((settings.segments_dir+"/"+file_name).c_str(), ios::trunc|ios::binary );
	}
	catch(...){
		error_log("Can't open segment file:"+settings.segments_dir+"/"+file_name);
		return 1;
	}
	try{
		easyhandle = curl_easy_init();
		if(easyhandle) {
			curl_easy_setopt(easyhandle, CURLOPT_URL, url.c_str() );
			curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, this);
			curl_easy_setopt(easyhandle, CURLOPT_PRIVATE, this);
			curl_easy_setopt(easyhandle, CURLOPT_RANGE, range.c_str());
			curl_easy_setopt(easyhandle, CURLOPT_TIMEOUT, settings.time_out);
			curl_easy_setopt(easyhandle, CURLOPT_FTP_RESPONSE_TIMEOUT, settings.ftp_response_timeout);
			curl_easy_setopt(easyhandle, CURLOPT_LOW_SPEED_LIMIT, settings.low_connection_speed_limit);
			curl_easy_setopt(easyhandle, CURLOPT_LOW_SPEED_TIME, settings.low_connection_speed_time);
			curl_easy_setopt(easyhandle, CURLOPT_MAX_RECV_SPEED_LARGE, settings.max_connection_speed);
			curl_easy_setopt(easyhandle, CURLOPT_USERAGENT, settings.user_agent.c_str());

			if (settings.proxy_off)
				curl_easy_setopt(easyhandle, CURLOPT_NOPROXY, "*");
			else{
				if ((settings.proxy_ip_or_name!="none") and (settings.proxy_port!=0)){
					curl_easy_setopt(easyhandle, CURLOPT_PROXY, settings.proxy_ip_or_name.c_str());
					curl_easy_setopt(easyhandle, CURLOPT_PROXYPORT, settings.proxy_port);
					debug("Using proxy:"+settings.proxy_ip_or_name+":"+toString(settings.proxy_port));
				}
				if (settings.proxy_user!="none"){
					curl_easy_setopt(easyhandle, CURLOPT_PROXYUSERNAME, settings.proxy_user.c_str());
					if (settings.proxy_password!="none")
						curl_easy_setopt(easyhandle, CURLOPT_PROXYPASSWORD, settings.proxy_password.c_str());
				}
			}

			if ((settings.bind_interface!="none") 
					and (settings.bind_interface!="") 
					and (settings.bind_interface!="NONE"))
				curl_easy_setopt(easyhandle, CURLOPT_INTERFACE, settings.bind_interface.c_str());
			//set connection timeout
			curl_easy_setopt(easyhandle, CURLOPT_CONNECTTIMEOUT, settings.connection_timeout);
			curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, write_data);
			curl_multi_add_handle(cm, easyhandle);
			return 0;
		}else
			return 1;
	}catch(...){
		error_log("Error in segment.cpp: add_easy_handle_to_multi()");
		return 1;
	}
}
void show_progress(double time_diff){
	try{
		stats.total_bytes_per_last_interval=0;
		for (uint con_num=0; con_num<MAX_CONNECTS; con_num++){
			//    ulong speed=bytes_written*1000/(diff_sec+diff_milli);
			//if connection is not NULL
			if (connection_array[con_num].segment){
				Tsegment* segment=(Tsegment*)connection_array[con_num].segment;
				stats.total_bytes_per_last_interval+=connection_array[con_num].get_bytes_per_last_interval();
				msg_segment_progress(con_num,segment->segment_num, segment->try_num,
					segment->downloaded_bytes,segment->segment_size,
					(connection_array[con_num].get_bytes_per_last_interval()*1000)/time_diff);
				connection_array[con_num].reset_bytes_per_last_interval();
			}
		}
		stats.last_time_interval=time_diff;
		stats.show_totals();
	}
	catch(...){
		error_log("Error in segment.cpp: show_progress()");
	}
}

size_t write_data(void *buffer, size_t size, size_t nmemb, void *cur_segment){
	ulong bytes_written=size*nmemb;
	try{
		Tsegment *segment;
		segment =(Tsegment*)cur_segment;
		segment->downloaded_bytes+=nmemb;

		try{
			segment->segment_file.write((char*)buffer,nmemb*size);
		}
		catch(...){
			error_log("Can't write segment file:"+segment->file_name);
		}
		connection_array[segment->connection_num].inc_bytes_per_last_interval(bytes_written);

		timeval now_time;
		gettimeofday(&now_time,NULL);
//	ulong diff_sec = difftime(now_time.tv_sec, prev_time.tv_sec) * 1000000;
//	ulong diff_milli = difftime(now_time.tv_usec, prev_time.tv_usec) + diff_sec;
		
		double time_diff_msecs=(now_time.tv_sec-stats.previous_time.tv_sec)*1000+(now_time.tv_usec-stats.previous_time.tv_usec)/1000;
//	debug(segment->file_name+"==="+toString((ulong)now_time)+"=="+toString(now_time));
		if (time_diff_msecs >= 500){
//		debug(segment->file_name+"--->"+toString((ulong)()));
			show_progress(time_diff_msecs);
			stats.previous_time=now_time;
		};
//	else 
//	debug(segment->file_name+"==="+toString(prev_time.tv_sec)+"=="+toString(prev_time.tv_usec)+"==="+toString((ulong)(diff_milli)));
	      //toString(diff_milli));
	//refresh();
	}
	catch(...){
		error_log("Error in segment.cpp: write_data()");
	}
	return bytes_written;
}
#endif