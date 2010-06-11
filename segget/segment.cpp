#include <sstream> 
#include <fstream>
#include <cstring>
#include <string>
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <curl/curl.h>

using namespace std;

template<typename T> std::string toString(T t) 
{ 
    std::stringstream s; 
    s << t; 
    return s.str(); 
} 
           
unsigned long downloaded_bytes=0;
size_t write_data(void *buffer, size_t size, size_t nmemb, void *cur_segment);

class Tsegment{
private:
  CURL *easyhandle;
  string file_name;
  char* urllist;
public:
  unsigned int segment_num;
  unsigned int segment_size;
  unsigned long downloaded_bytes;
  string url;
  string range;
  FILE *segment_file;
  Tsegment():easyhandle(0),file_name(""),urllist(0),segment_num(0),segment_size(1000),downloaded_bytes(0),url(""),range(""),segment_file(0){};
  Tsegment(const Tsegment &L);             // copy constructor
  Tsegment & operator=(const Tsegment &L);
  ~Tsegment();
  void set_segment(unsigned int seg_num, string segment_url, string segment_file_name, unsigned int seg_size, string segment_range);
  string get_file_name(){return file_name;};
  int add_easy_handle_to_multi(CURLM *cm);
};

void Tsegment::set_segment(unsigned int seg_num, string segment_url, string segment_file_name, unsigned int seg_size, string segment_range){
  segment_num=seg_num;
  segment_size=seg_size;
  url=segment_url;
  range=segment_range;
  downloaded_bytes=0;
  file_name=segment_file_name;
  //try
  segment_file = fopen(file_name.c_str(), "w" );
}
Tsegment::~Tsegment(){
	//try
		fclose(segment_file);
}

int Tsegment::add_easy_handle_to_multi(CURLM *cm){
//  CURLcode curl_result;

  easyhandle = curl_easy_init();
  cout << "Started downloading\n";

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
  cout << "DOWNLOADING:" << segment->get_file_name()<< " range:"<<segment->range<<" "<< segment->downloaded_bytes 
       << " b = " << (segment->downloaded_bytes*100/segment->segment_size) << "%\n";
  return bytes_written;
}
