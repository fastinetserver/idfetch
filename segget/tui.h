#ifndef __TUI_H__
#define __TUI_H__
#include "settings.h"

using namespace std;
template<typename T> string toString(T t);
template<typename T> string field(string prefix,T t, int width);

void msg_total(string msg_text);

void msg(uint y, uint x, string msg_text);
void msg_connecting(uint connection_num, uint distfile_num, uint segment_num, string msg_text);
void msg_segment_progress(uint connection_num, uint segment_num, uint try_num, ulong dld_bytes, ulong total_bytes, ulong speed);
void msg_status1(uint connection_num, uint segment_num, string msg_text);
void msg_status2(uint connection_num, string msg_text);
void msg_error(string error_text);
void msg_total(string msg_text);

void log(string log_msg_text);
void debug(string debug_msg_text);
void error_log(string error_msg_text);
void error_log_no_msg(string error_msg_text);
#endif