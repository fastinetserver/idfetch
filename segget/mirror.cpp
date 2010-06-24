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
		double	multiplier;
		Tmirror():
			active_num(0),
			dld_time(0),
			dld_size(1),
			multiplier(1){};
		void	start();
		void	stop(ulong time, uint size);
		double get_laziness_criterion();
		uint get_active_num(){return active_num;};
};

double Tmirror::get_laziness_criterion(){
	double criterion=multiplier*1000000000*dld_time/dld_size;
	multiplier=multiplier*100/(100+settings.benchmark_oblivion);
	return criterion;
}
void Tmirror::start(){
	if (!dld_time)
		dld_time=1;
	active_num++;
	multiplier=1;
}

void Tmirror::stop(ulong time, uint size){
	dld_time+=time;
	dld_size+=size;
	multiplier=1;
	debug(toString(time)+"---"+toString(size));
	active_num--;
}

map<string,Tmirror *> mirror_list;

Tmirror* find_mirror(string mirror_url){
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
		debug(" multiplier:"+toString(mirror_iterator->second->multiplier));
		debug("  criterion:"+toString(mirror_iterator->second->get_laziness_criterion()));
		return mirror_iterator->second;
	}
}


string strip_mirror_name(string path){
	string mirror_name;
	mirror_name=path.substr(0,path.find("/",(path.find("://",0)+3)));
	return mirror_name;
}

#endif