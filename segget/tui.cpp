//#include <sstream> 
//#include <fstream>
//#include <cstring>
//#include <string>
//#include <iostream>
//#include <stdio.h>
//#include <cstdio>

#include <string>
#include <ncurses.h>
using namespace std;
template<typename T> std::string toString(T t) 
{ 
    std::stringstream s; 
    s << t; 
    return s.str(); 
} 

template<typename T> std::string field(string prefix,T t, int width) 
{ 
    std::stringstream s1,s2;
    s1 << t;
    width=width+prefix.length();
    s2.width(width);
    s2 << prefix+s1.str();
    return s2.str(); 
} 


string set_width(string input_string, int width){
    std::stringstream s;
    s.width(width);
    s << input_string; 
    return s.str();
}

const uint CONNECTION_LINES=5;
void msg(uint y, uint x, string msg_text);

void msg(uint y, uint x, string msg_text){
     move(y,x);
     string ready_msg_text=msg_text+"                        ";
     printw(ready_msg_text.c_str());
     refresh();
}


void msg_connecting(uint connection_num, uint distfile_num, uint segment_num, string msg_text){
  msg(connection_num*CONNECTION_LINES+1,0,"DF#"+toString(distfile_num)+" Seg#"+toString(segment_num)+" "+msg_text);
}

void msg_segment_progress(uint connection_num, uint segment_num, uint try_num, ulong dld_bytes, ulong total_bytes, ulong speed){
  int percent=dld_bytes*100/total_bytes;
  msg(connection_num*CONNECTION_LINES,0,
	field("[",connection_num,2)+"]"
	+field(" Segment:",segment_num, 5)
	+field(" Try:",try_num,4)
	+field(" Bytes:",dld_bytes,7)
	+field(" / ",total_bytes,7)
	+field(" = ",percent,3)+"%%"
	+field(" Speed:",speed,7)+" Kb/s");
	
}

void msg_status1(uint connection_num, uint segment_num, string msg_text){
  msg(connection_num*CONNECTION_LINES+2,0,"Seg#"+toString(segment_num)+" "+msg_text);
}
void msg_status2(uint connection_num, string msg_text){
     msg(connection_num*CONNECTION_LINES+3,0,msg_text);
}
void msg_error(string error_text){
     msg(20,0, error_text);
}
void msg_total(string msg_text){
    msg(30,0,msg_text);
}
void log(string log_msg_text){
    ofstream file ("segget.log", ios::app);
    file << log_msg_text << endl;
    file.close();
    msg(31,0, "LOG:"+log_msg_text);
}
void debug(string debug_msg_text){
    ofstream file ("debug.log", ios::app);
    file << debug_msg_text << endl;
    file.close();
    msg(32,0, "DEBUG:"+debug_msg_text);
}
void error_log(string error_msg_text){
    ofstream file ("error.log", ios::app);
    file << error_msg_text << endl;
    file.close();
    msg(33,0, "ERROR:"+error_msg_text);
}
