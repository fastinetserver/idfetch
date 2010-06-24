#include "settings.h"

Tsettings settings;

void Tsettings::load_from_conf_file(){
	ConfigFile conf("segget.conf");
	conf.set(general_log_file,					"logs",				"general_log_file");
	conf.set(logs_dir,							"logs",				"logs_dir");
	conf.set(error_log_file,					"logs",				"error_log_file");
	conf.set(debug_log_file,					"logs",				"debug_log_file");

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

	conf.set(user_agent,						"user-data",		"user_agent");

	conf.set(proxy_ip_or_name,					"proxy",			"proxy_ip_or_name");
	conf.set(proxy_port,						"proxy",			"proxy_port");
	conf.set(proxy_off,							"proxy",			"proxy_off");
	conf.set(proxy_user,						"proxy",			"proxy_user");
	conf.set(proxy_password,					"proxy",			"proxy_password");
}