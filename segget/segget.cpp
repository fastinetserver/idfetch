#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <json/json.h>
#include <ncurses.h>
#include "pkg.cpp"
//#include "settings.cpp"

using namespace std;

uint pkg_num (0);
uint distfile_num(0);
uint segment_num(0);
Tpkg **Ppkg_array;

CURLM *cm;

void load_pkgs(){
	ifstream myfile("pkg.list");
	string buff((std::istreambuf_iterator<char>(myfile)), std::istreambuf_iterator<char>());
	myfile>>buff;
	myfile.close();
//	cout<< buff<<"\n";
        json_object *json_array_pkg_list;
        json_array_pkg_list = json_tokener_parse(buff.c_str());
        if (is_error(json_array_pkg_list))
		printf("error parsing json: %s\n",json_tokener_errors[-(unsigned long)json_array_pkg_list]);
	else {
		stats.pkg_count=json_object_array_length(json_array_pkg_list);
		Ppkg_array= new Tpkg* [stats.pkg_count];
		for(uint array_item_num=0;array_item_num<stats.pkg_count;array_item_num++){
			Ppkg_array[array_item_num]=new Tpkg;
			Ppkg_array[array_item_num]->load_pkg_from_json(json_object_array_get_idx(json_array_pkg_list,array_item_num));
		}
	}
}
void set_settings(){
  settings.set_resume(true);
  settings.distfiles_dir="./distfiles";  
  settings.segments_dir="./tmp";  
}
void show_pkgs(){
	for (uint array_item_num=0;array_item_num<stats.pkg_count;array_item_num++){
		cout <<"PKG:"<<array_item_num<<") cat:"<< Ppkg_array[array_item_num]->category <<" name:"<< Ppkg_array[array_item_num]->name <<"\n";
		for(uint distfile_array_item_num=0;distfile_array_item_num<Ppkg_array[array_item_num]->distfile_count;distfile_array_item_num++){
			cout << "    "<< distfile_array_item_num<<") distfile_name:"<< Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->name<<"\n";
			for(uint urllist_array_item_num=0;urllist_array_item_num<Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->url_count;urllist_array_item_num++){
				cout <<"        "<<Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->url_list[urllist_array_item_num]<<"\n";
			}
		}
	}
}

int choose_segment(uint connection_num){
  while (pkg_num<stats.pkg_count){
    debug("pkg_num:"+toString(pkg_num));
    while(distfile_num<Ppkg_array[pkg_num]->distfile_count){
      if (not(Ppkg_array[pkg_num]->Pdistfile_list[distfile_num]->downloaded)){
	debug("         distfile_num:"+toString(distfile_num));
	while (segment_num<Ppkg_array[pkg_num]->Pdistfile_list[distfile_num]->segments_count){
	  debug("                      segment_num:"+toString(segment_num));
	  //	segments_in_progress[connection_num]=
	  //	if not(Ppkg_array[pkg_num]->Pdistfile_list[distfile_num]->get_segment_downloaded_status(segment_num);
	  if (Ppkg_array[pkg_num]->Pdistfile_list[distfile_num]->provide_segment(cm, connection_num, segment_num)){
	    // segment already downloaded => go for the next one
	    segment_num++;
	  }
	  else{
	    segment_num++;
	    return 0;
	  }
	}
	segment_num=0;
      }
      distfile_num++;
    }
    distfile_num=0;
    pkg_num++;
  }
  //  for (uint array_item_num=0;array_item_num<pkg_count;array_item_num++){
  //for(int distfile_array_item_num=0;distfile_array_item_num<Ppkg_array[array_item_num]->distfile_count;distfile_array_item_num++){
  //  if (Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->url_count)
  //	Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->dosegments();
  //}
  //}
  return 1;
}
int download_pkgs(){
  //  cout << "Need to download: " << segments_count << " segments\n";

  CURLMsg *msg;
  long L;
  int M, Q, U = -1;
  fd_set R, W, E;
  struct timeval T;

  curl_global_init(CURL_GLOBAL_ALL);

  cm = curl_multi_init();

  /* we can optionally limit the total amount of connections this multi handle
     uses */
  curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, (long)MAX_CONNECTS);

  for (uint connection_num = 0; connection_num < MAX_CONNECTS; ++connection_num) {
    choose_segment(connection_num);
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
			string result_msg_text="RESULT:"+toString(msg->data.result)+" "+curl_easy_strerror(msg->data.result)+"while downloading segment";
			msg_status1(current_segment->connection_num,current_segment->segment_num,result_msg_text);
			curl_multi_remove_handle(cm, e);
			fclose(current_segment->segment_file);
			Tdistfile* prnt_distfile;
			prnt_distfile=(Tdistfile*)current_segment->parent_distfile;
			if (msg->data.result){
				// error -> start downloading again
				msg_status2(current_segment->connection_num, "Restarting "+current_segment->file_name);
				log("Restarting "+current_segment->file_name+" on connection#"+toString(current_segment->connection_num));
				prnt_distfile->provide_segment(cm,current_segment->connection_num,current_segment->segment_num);
				U++;
			}
			else{
				// no error => count this one and start new
				log("Succesfully downloaded "+current_segment->file_name+" on connection#"+toString(current_segment->connection_num));
				prnt_distfile->inc_dld_segments_count(current_segment);
				if (not choose_segment(current_segment->connection_num)) {
					U++; // just to prevent it from remaining at 0 if there are more URLs to get
				}
		}
		stats.show_totals();
		curl_easy_cleanup(e);
      }
      else {
	msg_error("ERROR: CURLMsg: "+msg->msg);
      }
    }
  }

  curl_multi_cleanup(cm);
  curl_global_cleanup();
  string em;
  debug("Exiting... (Press any key to exit) ");
  return EXIT_SUCCESS;
}

int main()
{
//  set_settings();
	prev_time=time((time_t *)NULL);
	initscr();
	curs_set(0);
	refresh();
	settings.load_from_conf_file();
	load_pkgs();
	//show_pkgs();
	stats.show_totals();
	download_pkgs();
	getch();
	endwin();
	return 0;
}
