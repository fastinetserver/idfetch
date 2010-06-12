#include <sstream> 
#include <fstream>
#include <cstring>
#include <string>
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <curl/curl.h>
#include "tui.cpp"
#include <ncurses.h>
//#include "distfile.cpp"
using namespace std;

typedef unsigned int uint;
           
#define MAX_CONNECTS 6 /* number of simultaneous transfers */

unsigned long downloaded_bytes=0;
size_t write_data(void *buffer, size_t size, size_t nmemb, void *cur_segment);

class Tsegment{
private:
  CURL *easyhandle;
  string file_name;
  char* urllist;
public:
  void* parent_distfile;
  uint connection_num;
  uint segment_num;
  uint segment_size;
  unsigned long downloaded_bytes;
  string url;
  string range;
  FILE *segment_file;
  Tsegment():easyhandle(0),file_name(""),urllist(0),parent_distfile(0),connection_num(0),segment_num(0),segment_size(1000),downloaded_bytes(0),
	     url(""),range(""),segment_file(0){};
  Tsegment(const Tsegment &L);             // copy constructor

  Tsegment & operator=(const Tsegment &L);
  ~Tsegment();
  void set_segment(void* prnt_distfile, uint seg_num, string distfile_name, unsigned int seg_size, string segment_range);
  void prepare_for_connection(CURLM *cm, uint con_num, string segment_url);
  string get_file_name(){return file_name;};
  int add_easy_handle_to_multi(CURLM *cm);
};

Tsegment *segments_in_progress[MAX_CONNECTS]={0};

void Tsegment::set_segment(void *prnt_distfile, uint seg_num, string distfile_name, uint seg_size, string segment_range){
  parent_distfile=prnt_distfile;
  segment_num=seg_num;
  segment_size=seg_size;
  range=segment_range;
  downloaded_bytes=0;
  file_name=distfile_name+".seg"+toString(seg_num);
  //try
}
void Tsegment::prepare_for_connection(CURLM *cm, uint con_num, string segment_url){
  msg_connecting(con_num,segment_num,"Downloading from "+segment_url);
  segments_in_progress[con_num]=this;  
  downloaded_bytes=0;
  connection_num=con_num;
  url=segment_url;
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
/*
void combine_segments(){
	cout << "Combining files \n";
	FILE* segment_file;
	FILE* complete_file;
	complete_file = fopen("dist.tar.bz2","w");
	char buffer[segment_size];
	for (int segment_num=0; segment_num <= segments_count; segment_num++){
		cout << "joining segment " << segment_num << "\n";
		segment_file = fopen(("distfile.seg"+toString(segment_num)).c_str(),"r");
		int read_bytes=fread(buffer, 1, segment_size, segment_file);
		fwrite(buffer,read_bytes, 1, complete_file);
		fclose(segment_file);
	}
	fclose(complete_file);
	cout << segments_count << " files combined\n";
};
*/

size_t write_data(void *buffer, size_t size, size_t nmemb, void *cur_segment){
  Tsegment *segment;
  segment =(Tsegment*)cur_segment;
  segment->downloaded_bytes+=nmemb;
  int bytes_written=fwrite(buffer,size,nmemb,segment->segment_file);

  //  for (uint con_num=0; con_num<MAX_CONNECTS; con_num++)
  //  if (segments_in_progress[con_num]){
      msg_segment_progress(segment->connection_num,segment->segment_num,segment->downloaded_bytes*100/segment->segment_size);
      //  }
      //    else
      // printw(" EEEEEEEEEEEEEEEEEEERRRRRRRRRRRRRRRROOOOOOOOOOOOOOOORRRRRRRRRRRRR n/a");
  /*
  cout << "Done:";
  for (uint con_num=0; con_num<MAX_CONNECTS; con_num++)
    if (segments_in_progress[con_num]){
      cout<<"S:";
      cout.width(4);
      cout<<segments_in_progress[con_num]->segment_num<<":";
      cout.width(3);
      cout<<segments_in_progress[con_num]->downloaded_bytes*100/segments_in_progress[con_num]->segment_size<<"% ";
    }
    else
      cout <<" n/a";
  
  cout <<"\n";
*/
  refresh();
  //  cout << "DOWNLOADING:" << segment->get_file_name()<< " range:"<<segment->range<<" "<< segment->downloaded_bytes 
  //     << " b = " << (segment->downloaded_bytes*100/segment->segment_size) << "%\n";
  return bytes_written;
}
