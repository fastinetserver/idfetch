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
	//mirrors
		uint max_connections_num_per_mirror;
		uint benchmark_oblivion;
	//proxy
		string proxy_ip_or_name;
		uint proxy_port;
		uint proxy_off;
		string proxy_user;
		string proxy_password;
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
			bind_interface("none"),
		//mirrors
			max_connections_num_per_mirror(2),
			benchmark_oblivion(5),
		//proxy
			proxy_ip_or_name("none"),
			proxy_port(0),
			proxy_off(1),
			proxy_user("none"),
			proxy_password("none")
		//logs
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

	conf.set(max_connections_num_per_mirror,	"mirrors",			"max_connections_num_per_mirror");
	conf.set(benchmark_oblivion,				"mirrors",			"benchmark_oblivion");

	conf.set(proxy_ip_or_name,					"proxy",			"proxy_ip_or_name");
	conf.set(proxy_port,						"proxy",			"proxy_port");
	conf.set(proxy_off,							"proxy",			"proxy_off");
	conf.set(proxy_user,						"proxy",			"proxy_user");
	conf.set(proxy_password,					"proxy",			"proxy_password");
}

Tsettings settings;
#endif