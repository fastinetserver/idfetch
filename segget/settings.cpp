#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <string>
#include "config.cpp"
//#include "tui.cpp"

using namespace std;

class Tsettings{
	private:
	public:
	//folders
		string distfiles_dir;
		string segments_dir;
	//distfiles
		uint max_connection_num_per_distfile;
	//segments
		uint max_segment_size;
		bool resume_on;
		uint max_tries;
	//connections
		uint max_connections;
		uint connection_timeout;
		uint ftp_response_timeout;
		uint time_out;
		uint low_connection_speed_limit;
		uint low_connection_speed_time;
		uint max_connection_speed;
		string bind_interface;
		Tsettings():
		//folders
			distfiles_dir("./distfiles"),
			segments_dir("./tmp"),
		//distfiles
			max_connection_num_per_distfile(3),
		//segments
			max_segment_size(500000),
			resume_on(1),
			max_tries(30),
		//connections
			max_connections(6),
			connection_timeout(15),
			ftp_response_timeout(5000),
			time_out(6000),
			low_connection_speed_limit(1000),
			low_connection_speed_time(10),
			max_connection_speed(0),
			bind_interface("none")
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
	
	conf.set(max_connection_num_per_distfile,	"distfiles",		"max_connection_num_per_distfile");

	conf.set(resume_on,							"segments",			"resume_on");
	conf.set(max_segment_size,					"segments",			"max_segment_size");
	conf.set(max_tries,							"segments",			"max_tries");

	conf.set(max_connections,					"connections",		"max_connections");
	conf.set(connection_timeout,				"connections",		"connection_timeout");
	conf.set(ftp_response_timeout,				"connections",		"ftp_response_timeout");
	conf.set(time_out,							"connections",		"timeout");
	conf.set(low_connection_speed_limit,		"connections",		"low_connection_speed_limit");
	conf.set(low_connection_speed_time,			"connections",		"low_connection_speed_time");
	conf.set(max_connection_speed,				"connections",		"max_connection_speed");
	conf.set(bind_interface,					"connections",		"bind_interface");
}

Tsettings settings;
#endif