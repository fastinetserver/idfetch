#ifndef __STR_H__
#define __STR_H__
#include <string>
#include <algorithm>
using namespace std;

int lower_char(int c)
{
  return tolower((unsigned char)c);
}

string noupper(string s){ 
	transform(s.begin(), s.end(), s.begin(), lower_char);
	return s; 
}
#endif