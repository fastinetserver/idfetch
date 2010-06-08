#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "json/json.h"
#include "segment.cpp"

using namespace std;

class Tdistfile{
	public:
		string *url_list;
		string name;
		int size;
		string RMD160;
		string SHA1;
		string SHA256;
		int url_count;
		void load_url_list(json_object* json_array_distfile_urllist);
                void dosegments();
		Tdistfile(json_object* json_obj_distfile);
		~Tdistfile();
};

void Tdistfile::load_url_list(json_object* json_array_distfile_urllist){
	url_count=json_object_array_length(json_array_distfile_urllist);
	url_list= new string [url_count];
	for(int urllist_array_item_num=0;urllist_array_item_num<url_count;urllist_array_item_num++){
		url_list[urllist_array_item_num]=json_object_get_string(json_object_array_get_idx(json_array_distfile_urllist,urllist_array_item_num));
	}
};

Tdistfile::Tdistfile(json_object* json_obj_distfile){
	name=json_object_to_json_string(json_object_object_get(json_obj_distfile,"name"));
	size=atoi(json_object_to_json_string(json_object_object_get(json_obj_distfile,"size")));
	RMD160=json_object_to_json_string(json_object_object_get(json_obj_distfile,"RMD160"));
	SHA1=json_object_to_json_string(json_object_object_get(json_obj_distfile,"SHA1"));
	SHA256=json_object_to_json_string(json_object_object_get(json_obj_distfile,"SHA256"));
	load_url_list(json_object_object_get(json_obj_distfile,"url_list"));
};
Tdistfile::~Tdistfile(){

	delete [] url_list;
}

void Tdistfile::dosegments()
{	
  int url_num=0;
  while (url_num<url_count){
    int segments_count=size/segment_size;
    cout << "Need to download: " << segments_count+1 << " segments\n";
    for (int segment_num=0; segment_num <= segments_count; segment_num++){
      int fetcher_result=1000;
      do {
	cout << "===Seg #"<<segment_num<< " of "<<segments_count<< "==========================\n";
	cout << "name:"<<name<<"\n";
	cout << "url:"<<url_list[url_num]<<"\n";
	int range_end;
	if (segment_num == segments_count)
	  range_end=size;
	else
	  range_end=(segment_num+1)*segment_size-1;
	Tsegment fetcher(url_list[url_num],"distfile.seg"+toString(segment_num), toString(segment_num*segment_size)+"-"+toString(range_end));
	cout << fetcher.get_file_name() << "\n";
	url_num++;
	//if (url_num >= sizeof(url_list)/sizeof(string))
	  //url_num=0;
	fetcher_result=fetcher.fetch();
      } while (fetcher_result);
    }
  }
}
