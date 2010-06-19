#ifndef __DISTFILE_H__
#define __DISTFILE_H__
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

ulong max_segment_size=1000*500;

using namespace std;

typedef unsigned int uint;

class Tdistfile{
  private:
    uint dld_segments_count;
  public:
    bool downloaded;
    string *url_list;
    uint url_num;
    uint segment_num;
    uint segments_count;
    Tsegment *dn_segments;
    string name;
    uint num;
    ulong size;
    string RMD160;
    string SHA1;
    string SHA256;
    uint url_count;
    uint segment_size;
    Tdistfile(): dld_segments_count(0), downloaded(0), url_list(0),url_num(0),segment_num(0),segments_count(0),
		dn_segments(0),name(""),num(0),size(0),
		RMD160(""),SHA1(""),SHA256(""),url_count(0),segment_size(max_segment_size){};
    Tdistfile(const Tdistfile &L);             // copy constructor
    Tdistfile & operator=(const Tdistfile &L);
    ~Tdistfile();

    void load_distfile_from_json(json_object* json_obj_distfile);
    void load_url_list(json_object* json_array_distfile_urllist);
    void split_into_segments();
    int provide_segment(CURLM* cm, uint connection_num, uint seg_num);
    void inc_dld_segments_count(Tsegment * current_segment);
    void combine_segments();
    bool check_if_dld();
};

void Tdistfile::load_url_list(json_object* json_array_distfile_urllist){
	url_count=json_object_array_length(json_array_distfile_urllist);
	url_list= new string [url_count];
	for(uint urllist_array_item_num=0;urllist_array_item_num<url_count;urllist_array_item_num++){
		url_list[urllist_array_item_num]=json_object_get_string(json_object_array_get_idx(json_array_distfile_urllist,urllist_array_item_num));
	}
	if (url_count){
	  num=++stats.distfiles_count;
	  stats.inc_total_size(size);
	}
}

bool Tdistfile::check_if_dld(){
 ifstream filec((settings.distfiles_dir+"/"+name).c_str());
// file.seekg (0);
 ulong start = filec.tellg();
 filec.seekg (0, ios::end);
 ulong end = filec.tellg();
 ulong d_size;
 d_size = end - start;
 //debug("seg:"+toString(seg_num)+" Dsize="+toString(downloaded_size)+" seg_size="+toString(segment_size));
 filec.close();
 if (d_size==size){
    downloaded=true;
    num=++stats.distfiles_count;
    stats.inc_dld_distfiles_count();
    stats.inc_dld_size(size);
    debug("Distfile:"+name+" already downloaded");
    return true;
 }
 else{
    debug("Distfile:"+name+" not downloaded");
    return false;
 }
}

void Tdistfile::load_distfile_from_json(json_object* json_obj_distfile){
	name=json_object_get_string(json_object_object_get(json_obj_distfile,"name"));
	size=atoi(json_object_to_json_string(json_object_object_get(json_obj_distfile,"size")));
	RMD160=json_object_get_string(json_object_object_get(json_obj_distfile,"RMD160"));
	SHA1=json_object_get_string(json_object_object_get(json_obj_distfile,"SHA1"));
	SHA256=json_object_get_string(json_object_object_get(json_obj_distfile,"SHA256"));

	if (not(check_if_dld())){
	  split_into_segments();
	  load_url_list(json_object_object_get(json_obj_distfile,"url_list"));
	}
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
    if (segment_num == (segments_count-1)){
      range_end=size-1;
      debug("last_segment ends"+toString(range_end));
    }
    else{
      range_end=(segment_num+1)*segment_size-1;
      debug("segment_range:"+toString(range_end));
    };
    dn_segments[segment_num].set_segment(this, segment_num, name, segment_size, range_end);
    if (dn_segments[segment_num].downloaded)
      inc_dld_segments_count(&dn_segments[segment_num]);
  }
}


Tdistfile::~Tdistfile(){
	delete [] url_list;
}

int Tdistfile::provide_segment(CURLM* cm, uint connection_num, uint seg_num){
  //  cout << "=Seg #"<<seg_num<< " of "<<segments_count<< " from " << "url:"<<url_list[url_num]<<"\n";
  if (dn_segments[seg_num].downloaded){
    msg_status2(connection_num, dn_segments[seg_num].file_name+" already downloaded");
    return 1;
  }
  else{
    dn_segments[seg_num].prepare_for_connection(cm, connection_num, num, url_list[url_num]);
    connection_array[connection_num].segment=&dn_segments[seg_num];
    url_num++;
    if (url_num >= url_count) 
      url_num=0;
    return 0;  
  }  
  //segment=dn_segments[seg_num];
}
void Tdistfile::inc_dld_segments_count(Tsegment* current_segment){
  stats.inc_dld_size(current_segment->segment_size);
  if (++dld_segments_count==segments_count)
    combine_segments();
}
void Tdistfile::combine_segments(){
  debug("Combining distfile"+name);
  ofstream distfile_file((settings.distfiles_dir+"/"+name).c_str(),ofstream::binary|ios::trunc);

	
  char * buffer;
  ulong cur_seg_size;

//  char * buffer;
//  buffer = new char [size];

  for (uint seg_num=0; seg_num < segments_count; seg_num++){
    debug("Joining "+name+" segment "+toString(seg_num)+"          ");
    ifstream segment_file((settings.segments_dir+"/"+dn_segments[seg_num].file_name).c_str(),ifstream::binary);

    // get size of file
    ulong start=segment_file.tellg();

    segment_file.seekg(0,ifstream::end);
    ulong end=segment_file.tellg();
    cur_seg_size=end-start;
    
    segment_file.seekg(0);
    debug(">>>>>SEG:"+dn_segments[seg_num].file_name+" Start: "+toString(start)+" End: "+toString(end)+" Size: "+toString(cur_seg_size));

    // allocate memory for file content
    buffer = new char [cur_seg_size];

    // read content of infile
    segment_file.read (buffer,cur_seg_size);

    // write to outfile
    distfile_file.write (buffer,cur_seg_size);
  
    // release dynamically-allocated memory
    delete[] buffer;
    segment_file.close();
    if(remove((settings.segments_dir+"/"+dn_segments[seg_num].file_name).c_str()) != 0 )      error_log("Tdistfile::combine_segments() - Error: can't delete:"+settings.segments_dir+"/"+dn_segments[seg_num].file_name);
    else
      debug(settings.segments_dir+"/"+dn_segments[seg_num].file_name+" deleted" );


  }
  distfile_file.close();
  stats.inc_dld_distfiles_count();
  log("Distfile "+name+" has been combined");

	if (rmd160_ok(settings.distfiles_dir+"/"+name,RMD160))
		log("RMD160 checksum for distfile:"+name+" is [OK]");
	else{
		log("Error: RMD160 checksum for distfile:"+name+" [FAILED]");
		error_log("Error: RMD160 checksum for distfile:"+name+" [FAILED]");
	}

	if (sha1_ok(settings.distfiles_dir+"/"+name,SHA1))
		log("SHA1   checksum for distfile:"+name+" is [OK]");
	else{
		log("Error: SHA1   checksum for distfile:"+name+" [FAILED]");
		error_log("Error: SHA1   checksum for distfile:"+name+" [FAILED]");
	}

	if (sha256_ok(settings.distfiles_dir+"/"+name,SHA256))
		log("SHA256 checksum for distfile:"+name+" is [OK]");
	else{
		log("Error: SHA256 checksum for distfile:"+name+" [FAILED]");
		error_log("Error: SHA256 checksum for distfile:"+name+" [FAILED]");
	}

}
#endif