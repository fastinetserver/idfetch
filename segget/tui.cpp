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

const uint CONNECTION_LINES=4;
void msg(uint y, uint x, string msg_text);

void msg(uint y, uint x, string msg_text){
     move(y,x);
     string ready_msg_text=msg_text+"                                                                                                               ";
     printw(ready_msg_text.c_str());
     refresh();
}


void msg_connecting(uint connection_num, uint segment_num, string msg_text){
  msg(connection_num*CONNECTION_LINES,0,"Seg#"+toString(segment_num)+" "+msg_text);
}

void msg_segment_progress(uint connection_num, uint segment_num, uint percent){
     msg(connection_num*CONNECTION_LINES+1,0,toString(connection_num)+") [S:"+toString(segment_num)+"] "+toString(percent)+" percent");
}

void msg_status1(uint connection_num, uint segment_num, string msg_text){
  msg(connection_num*CONNECTION_LINES+2,0,"Seg#"+toString(segment_num)+" "+msg_text);
}
void msg_status2(uint connection_num, uint segment_num, string msg_text){
     msg(connection_num*CONNECTION_LINES+3,0,"Seg#"+toString(segment_num)+" "+msg_text);
}
void msg_error(string error_text){
     msg(20,0, error_text);
}
void msg_total(string total_msg_text){
     msg(24,0, total_msg_text);
}
