#include "tui.h"
extern Tsettings settings;

const uint CONNECTION_LINES=5;

template<typename T> string toString(T t)
{
	stringstream s;
	s << t;
	return s.str();
}

template<typename T> string field(string prefix,T t, int width) 
{
	stringstream s1,s2;
	s1 << t;
	width=width+prefix.length();
	s2.width(width);
	s2 << prefix+s1.str();
	return s2.str();
}

void msg(uint y, uint x, string msg_text){
		move(y,x);
		string ready_msg_text=msg_text+"                        ";
		printw(ready_msg_text.c_str());
		refresh();
}

void msg_connecting(uint connection_num, uint distfile_num, uint segment_num, string msg_text){
	try{
		msg(connection_num*CONNECTION_LINES+1,0,"DF#"+toString(distfile_num)+" Seg#"+toString(segment_num)+" "+msg_text);
	}
	catch(...)
	{
		error_log_no_msg("Error in tui.cpp: msg_connecting()");
	}
}

void msg_segment_progress(uint connection_num, uint segment_num, uint try_num, ulong dld_bytes, ulong total_bytes, ulong speed){
	try{
		int percent=dld_bytes*100/total_bytes;
		if (speed<1000)
			  msg(connection_num*CONNECTION_LINES,0,
			field("[",connection_num,2)+"]"
			+field(" Segment:",segment_num, 5)
			+field(" Try:",try_num,4)
			+field(" Bytes:",dld_bytes,7)
			+field(" / ",total_bytes,7)
			+field(" = ",percent,3)+"%%"
			+field(" Speed:",speed,7)+" b/s");
		else
			msg(connection_num*CONNECTION_LINES,0,
			field("[",connection_num,2)+"]"
			+field(" Segment:",segment_num, 5)
			+field(" Try:",try_num,4)
			+field(" Bytes:",dld_bytes,7)
			+field(" / ",total_bytes,7)
			+field(" = ",percent,3)+"%%"
			+field(" Speed:",speed/1000,7)+" Kb/s");
	}
	catch(...)
	{
		error_log_no_msg("Error in tui.cpp: msg_segment_progress()");
	}
}

void msg_status1(uint connection_num, uint segment_num, string msg_text){
	try{
		msg(connection_num*CONNECTION_LINES+2,0,"Seg#"+toString(segment_num)+" "+msg_text);
	}
	catch(...)
	{
		error_log_no_msg("Error in tui.cpp: msg_status1()");
	}
}
void msg_status2(uint connection_num, string msg_text){
	try{
		msg(connection_num*CONNECTION_LINES+3,0,msg_text);
	}
	catch(...)
	{
		error_log_no_msg("Error in tui.cpp: msg_status2()");
	}
}
void msg_error(string error_text){
	try{
		msg(20,0, error_text);
	}
	catch(...)
	{
		error_log_no_msg("Error in tui.cpp: msg_error()");
	}
}
void msg_total(string msg_text){
	try{
		msg(30,0,msg_text);
	}
	catch(...)
	{
		error_log_no_msg("Error in tui.cpp: msg_total()");
	}
}
void log(string log_msg_text){
	msg(31,0, "LOG:"+log_msg_text);
	ofstream file;
	file.exceptions (ofstream::failbit | ofstream::badbit);
	try{
		file.open((settings.logs_dir+"/"+settings.general_log_file).c_str(), ios::app);
	}
	catch(...){
		error_log("Error opening "+settings.logs_dir+"/"+settings.general_log_file+".");
		return;
	}
	try{
		file << log_msg_text << endl;
		file.close();
	}
	catch(...){
		error_log("Error while writing "+settings.logs_dir+"/"+settings.general_log_file+".");
		return;
	}
}
void debug(string debug_msg_text){
	msg(32,0, "DEBUG:"+debug_msg_text);
	ofstream file;
	file.exceptions (ofstream::failbit | ofstream::badbit);
	try{
		file.open((settings.logs_dir+"/"+settings.debug_log_file).c_str(), ios::app);
	}
	catch(...){
		error_log("Error opening "+settings.logs_dir+"/"+settings.debug_log_file+".");
		return;
	}
	try{
		file << debug_msg_text << endl;
		file.close();
	}
	catch(...){
		error_log("Error while writing "+settings.logs_dir+"/"+settings.debug_log_file+".");
		return;
	}
}

void error_log_no_msg(string error_msg_text){
	ofstream file ((settings.logs_dir+"/"+settings.error_log_file).c_str(), ios::app);
	file << error_msg_text << endl;
	file.close();
}

void error_log(string error_msg_text){
	ofstream file ((settings.logs_dir+"/"+settings.error_log_file).c_str(), ios::app);
	file << error_msg_text << endl;
	file.close();
	try{
		msg(33,0, "ERROR:"+error_msg_text);
		}
	catch(...)
	{
		error_log_no_msg("Error calling msg() in tui.cpp: error_log()");
	}

}