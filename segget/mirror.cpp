#ifndef __MIRROR_BENCHMARKER_H__
#define __MIRROR_BENCHMARKER_H__

#include <string>
#include <map>
#include "str.cpp"
#include "tui.h"
#include "settings.cpp"
using namespace std;

extern Tsettings settings;
class Tmirror{
	private:
		uint active_num;
	public:
		ulong	dld_time;
		ulong	dld_size;
		double	honesty;
		Tmirror():
			active_num(0),
			dld_time(0),
			dld_size(1),
			honesty(1){};
		void	start();
		void	stop(ulong time, uint size);
		double mirror_on_the_wall();
		uint get_active_num(){return active_num;};
};

map<string,Tmirror *> mirror_list;

double Tmirror::mirror_on_the_wall(){
	try{
		double criterion=honesty*1000000000*dld_time/dld_size;
		honesty=honesty*100/(100+settings.benchmark_oblivion);
		return criterion;
	}catch(...){
		error_log("Error in mirror.cpp: mirror_on_the_wall()");
		return 0;
	}
}
void Tmirror::start(){
	try{
		if (!dld_time)
			dld_time=1;
		active_num++;
		honesty=1;
	}catch(...){
		error_log("Error in mirror.cpp: start()");
	}
}

void Tmirror::stop(ulong time, uint size){
	try{
		dld_time+=time;
		dld_size+=size;
		honesty=1;
		debug(toString(time)+"---"+toString(size));
		active_num--;
	}catch(...){
		error_log("Error in mirror.cpp: stop()");
	}
}

Tmirror* find_mirror(string mirror_url){
	try{
		map<string,Tmirror*>::const_iterator mirror_iterator = mirror_list.find(mirror_url);
		if (mirror_iterator==mirror_list.end()){
			Tmirror * Pnew_mirror=new Tmirror;
			debug("Cant find mirror:"+mirror_url+" - creating new record");
			mirror_list[mirror_url]=Pnew_mirror;
			return Pnew_mirror;
		}
		else{
			debug("Found mirror:"+mirror_url);
			debug("==================>>");
			debug("       time:"+toString(mirror_iterator->second->dld_time));
			debug("       size:"+toString(mirror_iterator->second->dld_size));
			debug("    honesty:"+toString(mirror_iterator->second->honesty));
			debug("  criterion:"+toString(mirror_iterator->second->mirror_on_the_wall()));
			return mirror_iterator->second;
		}
	}catch(...){
		error_log("Error in mirror.cpp: find_mirror()");
		return 0;
	}
}

string strip_mirror_name(string path){
	try{
		string mirror_name;
		mirror_name=path.substr(0,path.find("/",(path.find("://",0)+3)));
		return mirror_name;
	}catch(...){
		error_log("Error in mirror.cpp: strip_mirror_name()");
		return "";
	}
}
#endif