#include <sstream> 
#include <fstream>
#include <cstring>
#include <string>
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <curl/curl.h>

int segment_size=20000;

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
		string file_name;
		char* urllist[];
	public:
		unsigned long downloaded_bytes;
		string url;
		string range;
		FILE *segment_file;
		Tsegment(string segment_url, string segment_file_name, string segment_range);
		~Tsegment();
		string get_file_name(){return file_name;};
		int fetch();
		
};

Tsegment::Tsegment(string segment_url, string segment_file_name, string segment_range){
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



int Tsegment::fetch(){
  CURL *easyhandle;
  CURLcode result;

  easyhandle = curl_easy_init();
  cout << "Started downloading\n";
  //  cout << "UUURRLL:" <<url.c_str()<<"\n";
  //  cout << "range:" <<range <<"\n";
  if(easyhandle) {
    curl_easy_setopt(easyhandle, CURLOPT_URL, url.c_str() );
    //    curl_easy_setopt(easyhandle, CURLOPT_URL, "http://www.mail.ru");
    curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(easyhandle, CURLOPT_RANGE, range.c_str());
    curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, write_data);
    result = curl_easy_perform(easyhandle);

    cout << "RESULT:" << result << "\n";
     // always cleanup
    curl_easy_cleanup(easyhandle);
    return result;
  }
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
  fwrite(buffer,size,nmemb,segment->segment_file);
  cout << "DOWNLOADING:" << segment->get_file_name()<< " range:"<<segment->range<<" " << segment->downloaded_bytes << " b = " << (segment->downloaded_bytes*100/segment_size) << "%\n";
  return nmemb;
}
