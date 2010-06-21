#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <string>
#include "config.cpp"
//#include "tui.cpp"

using namespace std;

class Tsettings{
	private:
		bool resume_on;
	public:
		string distfiles_dir;
		string segments_dir;
		uint max_connections;
		uint max_connection_num_per_distfile;
		uint max_segment_size;
		uint max_tries;
		Tsettings():
			resume_on(1),
			distfiles_dir("./distfiles"),
			segments_dir("./tmp"),
			max_connections(6),
			max_connection_num_per_distfile(3),
			max_segment_size(500000),
			max_tries(30)
			{};
		void set_resume(bool resume_setting){resume_on=resume_setting;};
		bool get_resume(){return resume_on;};
		void load_from_conf_file();
};

void Tsettings::load_from_conf_file(){
	ConfigFile conf("segget.conf");
	conf.set(distfiles_dir,			"folders",			"distfiles_dir");
	conf.set(segments_dir,			"folders",			"segments_dir");
//	log("segments_dir set to:"+segments_dir);
	conf.set(max_connections,		"connections",		"max_connections");
	conf.set(max_connection_num_per_distfile,		"distfiles",		"max_connection_num_per_distfile");
	conf.set(resume_on,				"segments",			"resume_on");
	conf.set(max_segment_size,		"segments",			"max_segment_size");
	conf.set(max_tries,				"segments",			"max_tries");
}

Tsettings settings;
#endif