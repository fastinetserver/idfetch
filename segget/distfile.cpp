#include <errno.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#  include <unistd.h>
#endif
#include <curl/multi.h>

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <json/json.h>
#include "segment.cpp"

int max_segment_size=1000*500;

using namespace std;

typedef unsigned int uint;

class Tdistfile{
  private:
    uint dld_segments_count;
  public:
    string *url_list;
    uint url_num;
    uint segment_num;
    uint segments_count;
    Tsegment *dn_segments;
    string name;
    int size;
    string RMD160;
    string SHA1;
    string SHA256;
    uint url_count;
    uint segment_size;
    Tdistfile(): dld_segments_count(0), url_list(0),url_num(0),segment_num(0),segments_count(0),dn_segments(0),name(""),size(0),
		RMD160(""),SHA1(""),SHA256(""),url_count(0),segment_size(max_segment_size){};
    Tdistfile(const Tdistfile &L);             // copy constructor
    Tdistfile & operator=(const Tdistfile &L);
    ~Tdistfile();
    void load_distfile_from_json(json_object* json_obj_distfile);
    void load_url_list(json_object* json_array_distfile_urllist);
    void split_into_segments();
    int provide_segment(CURLM* cm, uint con_num, uint seg_num);
};

void Tdistfile::load_url_list(json_object* json_array_distfile_urllist){
	url_count=json_object_array_length(json_array_distfile_urllist);
	url_list= new string [url_count];
	for(uint urllist_array_item_num=0;urllist_array_item_num<url_count;urllist_array_item_num++){
		url_list[urllist_array_item_num]=json_object_get_string(json_object_array_get_idx(json_array_distfile_urllist,urllist_array_item_num));
	}
}

void Tdistfile::load_distfile_from_json(json_object* json_obj_distfile){
	name=json_object_get_string(json_object_object_get(json_obj_distfile,"name"));
	size=atoi(json_object_to_json_string(json_object_object_get(json_obj_distfile,"size")));
	split_into_segments();
	RMD160=json_object_get_string(json_object_object_get(json_obj_distfile,"RMD160"));
	SHA1=json_object_get_string(json_object_object_get(json_obj_distfile,"SHA1"));
	SHA256=json_object_get_string(json_object_object_get(json_obj_distfile,"SHA256"));
	load_url_list(json_object_object_get(json_obj_distfile,"url_list"));
}

void Tdistfile::split_into_segments(){
  int range_end;
  if (size>max_segment_size){
    segments_count=size/max_segment_size;
    // set segments to have equal size, only last segment may be somewhat large (up to segment_count bytes large)
    segment_size=size/segments_count;	  
  }
  else{
    segment_size=size;
    segments_count=1;
  };
  dn_segments = new Tsegment [segments_count];
  //none downloaded yet
  for (uint segment_num=0; segment_num<segments_count; segment_num++){
    if (segment_num == segments_count)
      range_end=size;
    else
      range_end=(segment_num+1)*segment_size-1;
    dn_segments[segment_num].set_segment(this, segment_num, name, segment_size, toString(segment_num*segment_size)+"-"+toString(range_end));
  }
}


Tdistfile::~Tdistfile(){
	delete [] url_list;
}

int Tdistfile::provide_segment(CURLM* cm, uint con_num, uint seg_num)
{
  //  cout << "=Seg #"<<seg_num<< " of "<<segments_count<< " from " << "url:"<<url_list[url_num]<<"\n";
  if (dn_segments[seg_num].downloaded)
    return 1;
  else{
    url_num++;
    if (url_num >= url_count) 
      url_num=0;
    dn_segments[seg_num].prepare_for_connection(cm, con_num, url_list[url_num]);
    connection_array[con_num].segment=&dn_segments[seg_num];
    return 0;  
  }  
  //segment=dn_segments[seg_num];
}
