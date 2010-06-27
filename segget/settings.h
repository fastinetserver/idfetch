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
	//user-data
		string user_agent;
	//proxy
		string proxy_ip_or_name;
		uint proxy_port;
		bool proxy_off;
		string proxy_user;
		string proxy_password;
	//logs
		string logs_dir;
		string general_log_file;
		string error_log_file;
		string debug_log_file;

	Tsettings():
		//folders
			distfiles_dir("./distfiles"),
			segments_dir("./tmp"),
		//distfiles
			max_connection_num_per_distfile(3),
		//segments
			max_segment_size(500000),
			resume_on(1),
			max_tries(10),
		//connections
			max_connections(6),
			connection_timeout(15),
			ftp_response_timeout(180),
			time_out(500),
			low_connection_speed_limit(1000),
			low_connection_speed_time(10),
			max_connection_speed(0),
			bind_interface("none"),
		//mirrors
			max_connections_num_per_mirror(1),
			benchmark_oblivion(5),
		//user-data
			user_agent("segget"),
		//proxy
			proxy_ip_or_name("none"),
			proxy_port(3128),
			proxy_off(1),
			proxy_user("none"),
			proxy_password("none"),
		//logs
			logs_dir("./logs"),
			general_log_file("segget.log"),
			error_log_file("error.log"),
			debug_log_file("debug.log")
			{};
		void set_resume(bool resume_setting){resume_on=resume_setting;};
		bool get_resume(){return resume_on;};
		void load_from_conf_file();
};

#endif
