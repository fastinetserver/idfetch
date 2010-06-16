#include <sstream> 
#include <fstream>
#include <cstring>
#include <string>
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <ncurses.h>
#include <curl/curl.h>
#include "settings.cpp"
#include "stats.cpp"

using namespace std;

unsigned long downloaded_bytes=0;
size_t write_data(void *buffer, size_t size, size_t nmemb, void *cur_segment);

class Tsegment{
private:
  CURL *easyhandle;
  char* urllist;
public:
  string file_name;
  bool downloaded;
  uint try_num;
  void* parent_distfile;
  uint connection_num;
  uint segment_num;
  uint segment_size;
  unsigned long downloaded_bytes;
  string url;
  string range;
  FILE *segment_file;
  Tsegment():easyhandle(0),urllist(0),file_name(""), downloaded (0),try_num(0),parent_distfile(0),connection_num(0),segment_num(0),segment_size(1000),downloaded_bytes(0),
	     url(""),range(""),segment_file(0){};
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
  parent_distfile=prnt_distfile;
  segment_num=seg_num;
  segment_size=range_end-seg_num*default_seg_size+1;
  range=toString(seg_num*default_seg_size)+"-"+toString(range_end);
  file_name="./tmp/"+distfile_name+".seg"+toString(seg_num);
  ulong downloaded_size;
  if (settings.get_resume()){
    //check if downloaded
    ifstream file(file_name.c_str(), ios::in|ios::binary);
    ulong start = file.tellg();
    file.seekg (0, ios::end);
    ulong end = file.tellg();
    downloaded_size = end - start;
    debug("seg:"+toString(seg_num)+" Dsize="+toString(downloaded_size)+" seg_size="+toString(segment_size));
    file.close();
    if (downloaded_size==segment_size){
      downloaded=true;
      debug("seg:"+toString(seg_num)+" Downloaded");
    }
    else{
      debug("seg:"+toString(seg_num)+" not downloaded");
      downloaded_bytes=0;
    }
  }
  //try
}
void Tsegment::prepare_for_connection(CURLM *cm, uint con_num, uint distfile_num, string segment_url){
  msg_connecting(con_num,distfile_num, segment_num,"Downloading from "+segment_url);
  segments_in_progress[con_num]=this;  
  downloaded_bytes=0;
  connection_num=con_num;
  url=segment_url;
  try_num++;
  add_easy_handle_to_multi(cm);
}

Tsegment::~Tsegment(){
	//try
		fclose(segment_file);
}

int Tsegment::add_easy_handle_to_multi(CURLM *cm){
//  CURLcode curl_result;

  segment_file = fopen(file_name.c_str(), "w" );

  easyhandle = curl_easy_init();
  //  cout << "Started downloading\n";

  //  curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, cb);
  //  curl_easy_setopt(eh, CURLOPT_HEADER, 0L);
  //  curl_easy_setopt(eh, CURLOPT_URL, urls[i]);
  //  curl_easy_setopt(eh, CURLOPT_VERBOSE, 0L);


  //  cout << "UUURRLL:" <<url.c_str()<<"\n";
  //  cout << "range:" <<range <<"\n";
  if(easyhandle) {
    curl_easy_setopt(easyhandle, CURLOPT_URL, url.c_str() );
    //    curl_easy_setopt(easyhandle, CURLOPT_URL, "http://www.mail.ru");
    curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(easyhandle, CURLOPT_PRIVATE, this);
    curl_easy_setopt(easyhandle, CURLOPT_RANGE, range.c_str());
    curl_easy_setopt(easyhandle, CURLOPT_TIMEOUT, 60*100);
    curl_easy_setopt(easyhandle, CURLOPT_FTP_RESPONSE_TIMEOUT, 60*80);

    //set connection timeout
    curl_easy_setopt(easyhandle, CURLOPT_CONNECTTIMEOUT, 15);
    curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, write_data);

    curl_multi_add_handle(cm, easyhandle);
    return 0;
  }
  else
    return 1;
}
void show_progress(double time_left){
  stats.total_bytes_per_last_interval=0;
  for (uint con_num=0; con_num<MAX_CONNECTS; con_num++){
//    ulong speed=bytes_written*1000/(diff_sec+diff_milli);
		//if connection is not NULL
		if (connection_array[con_num].segment){
			Tsegment* segment=(Tsegment*)connection_array[con_num].segment;
			stats.total_bytes_per_last_interval+=connection_array[con_num].get_bytes_per_last_interval();
			msg_segment_progress(con_num,segment->segment_num, segment->try_num,
				segment->downloaded_bytes,segment->segment_size,
				connection_array[con_num].get_bytes_per_last_interval()*1000/time_left);
			connection_array[con_num].reset_bytes_per_last_interval();
		}
  }
  stats.last_time_interval=time_left;
  stats.show_totals();
  
  
}


size_t write_data(void *buffer, size_t size, size_t nmemb, void *cur_segment){
  Tsegment *segment;
  segment =(Tsegment*)cur_segment;
  segment->downloaded_bytes+=nmemb;
  ulong bytes_written=fwrite(buffer,size,nmemb,segment->segment_file);
  
  connection_array[segment->connection_num].inc_bytes_per_last_interval(bytes_written);
    
  struct timeval now_time;
  gettimeofday(&now_time, NULL);
  double diff_sec = difftime(now_time.tv_sec, prev_time.tv_sec) * 1000000;
  double diff_milli = difftime(now_time.tv_usec, prev_time.tv_usec) + diff_sec;
  if (diff_milli > 1000000){
      show_progress(diff_milli);
      prev_time=now_time;
  }

  return bytes_written;
}
