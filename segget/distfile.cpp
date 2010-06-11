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

int max_segment_size=512000;

#define MAX 4 /* number of simultaneous transfers */

CURLM *cm;


using namespace std;

class Tdistfile{
public:
  string *url_list;
  unsigned int url_num;
  unsigned int segment_num;
  unsigned int segments_count;
  Tsegment *dn_segments;
  string name;
  int size;
  string RMD160;
  string SHA1;
  string SHA256;
  int url_count;
  unsigned int segment_size;
  Tdistfile(): url_list(0),url_num(0),segment_num(0),segments_count(0),dn_segments(0),name(""),size(0),
	       RMD160(""),SHA1(""),SHA256(""),url_count(0),segment_size(max_segment_size){};
  Tdistfile(const Tdistfile &L);             // copy constructor
  Tdistfile & operator=(const Tdistfile &L);
  ~Tdistfile();
  void load_distfile_from_json(json_object* json_obj_distfile);
  void load_url_list(json_object* json_array_distfile_urllist);
  int dosegments();
  void setsegment(unsigned int seg_num);
};

void Tdistfile::load_url_list(json_object* json_array_distfile_urllist){
	url_count=json_object_array_length(json_array_distfile_urllist);
	url_list= new string [url_count];
	for(int urllist_array_item_num=0;urllist_array_item_num<url_count;urllist_array_item_num++){
		url_list[urllist_array_item_num]=json_object_get_string(json_object_array_get_idx(json_array_distfile_urllist,urllist_array_item_num));
	}
}

void Tdistfile::load_distfile_from_json(json_object* json_obj_distfile){
	name=json_object_get_string(json_object_object_get(json_obj_distfile,"name"));
	size=atoi(json_object_to_json_string(json_object_object_get(json_obj_distfile,"size")));
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
	//	for (int segment_num=0; segment_num<segments_count; segment_num++)
	//dn_segments[segment_num] = false;
	RMD160=json_object_get_string(json_object_object_get(json_obj_distfile,"RMD160"));
	SHA1=json_object_get_string(json_object_object_get(json_obj_distfile,"SHA1"));
	SHA256=json_object_get_string(json_object_object_get(json_obj_distfile,"SHA256"));
	load_url_list(json_object_object_get(json_obj_distfile,"url_list"));
}
Tdistfile::~Tdistfile(){
	delete [] url_list;
}

void Tdistfile::setsegment(unsigned int seg_num)
{
  cout << "=Seg #"<<seg_num<< " of "<<segments_count<< "=>" << "url:"<<url_list[url_num]<<"\n";
  int range_end;
  
  if (seg_num == segments_count)
    range_end=size;
  else
    range_end=(seg_num+1)*segment_size-1;
  
  dn_segments[seg_num].set_segment(seg_num, url_list[url_num], name+".seg"+toString(seg_num), segment_size, toString(seg_num*segment_size)
				   +"-"+toString(range_end));
  cout << dn_segments[seg_num].get_file_name() << "\n";
  url_num++;
  if (url_num >= sizeof(url_list)/sizeof(string)) url_num=0;
   dn_segments[seg_num].add_easy_handle_to_multi(cm);
}


int Tdistfile::dosegments()
{	
  cout << "Need to download: " << segments_count << " segments\n";

  CURLMsg *msg;
  long L;
  int M, Q, U = -1;
  fd_set R, W, E;
  struct timeval T;

  curl_global_init(CURL_GLOBAL_ALL);

  cm = curl_multi_init();

  /* we can optionally limit the total amount of connections this multi handle
     uses */
  curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, (long)MAX);

  for (segment_num = 0; segment_num < MAX; ++segment_num) {
    setsegment(segment_num);
  };
  
  while (U) {
    while (CURLM_CALL_MULTI_PERFORM == curl_multi_perform(cm, &U)){};

    if (U) {
      FD_ZERO(&R);
      FD_ZERO(&W);
      FD_ZERO(&E);

      if (curl_multi_fdset(cm, &R, &W, &E, &M)) {
        fprintf(stderr, "E: curl_multi_fdset\n");
        return EXIT_FAILURE;
      }

      if (curl_multi_timeout(cm, &L)) {
        fprintf(stderr, "E: curl_multi_timeout\n");
        return EXIT_FAILURE;
      }
      if (L == -1)
        L = 100;

      if (M == -1) {
#ifdef WIN32
        Sleep(L);
#else
        sleep(L / 1000);
#endif
      } else {
        T.tv_sec = L/1000;
        T.tv_usec = (L%1000)*1000;

        if (0 > select(M+1, &R, &W, &E, &T)) {
          fprintf(stderr, "E: select(%i,,,,%li): %i: %s\n",
              M+1, L, errno, strerror(errno));
          return EXIT_FAILURE;
        }
      }
    }

    while ((msg = curl_multi_info_read(cm, &Q))) {
      if (msg->msg == CURLMSG_DONE) {
        Tsegment *current_segment;
        CURL *e = msg->easy_handle;
        curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &current_segment);
        fprintf(stderr, "R: %d - %s while downloading segment #<%d>\n",
                msg->data.result, curl_easy_strerror(msg->data.result), current_segment->segment_num);
        curl_multi_remove_handle(cm, e);
	if (msg->data.result){
	  // error -> start downloading again
	  cout << "Restarting segment #"<<current_segment->segment_num<<"\n";
	  setsegment(current_segment->segment_num);
	  U++;
	}
	else{
	  if (segment_num < segments_count) {
	    // no error - start new one
	    setsegment(segment_num);
	    segment_num++;
	    U++; // just to prevent it from remaining at 0 if there are more URLs to get
	  }
	}
        curl_easy_cleanup(e);
      }
      else {
	fprintf(stderr, "E: CURLMsg (%d)\n", msg->msg);
      }
    }
  }

  curl_multi_cleanup(cm);
  curl_global_cleanup();
  string em;
  cout << "exit loop (yes)?\n";
  cin >> em;
  return EXIT_SUCCESS;
}
