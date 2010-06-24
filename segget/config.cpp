#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <map>
#include "str.cpp"
#include "tui.h"
using namespace std;

map<string,string> content_;

class ConfigFile {

public:
	
	ConfigFile(std::string const& configFile);

	int set(string &dst,string const& section, string const& entry) const;
	int set(uint &dst,string const& section, string const& entry) const;
	int set(bool &dst,string const& section, string const& entry) const;
};

#endif
#include <fstream>

string trim(std::string const& source, char const* delims = " \t\r\n") {
	string result(source);
	string::size_type index = result.find_last_not_of(delims);
	if(index != string::npos)
		result.erase(++index);

	index = result.find_first_not_of(delims);
	if(index != string::npos)
		result.erase(0, index);
	else
		result.erase();
	return result;
}

ConfigFile::ConfigFile(string const& configFile) {
	ifstream file(configFile.c_str());
	string line;
	string name;
	string value;
	string inSection;
	int posEqual;
	while (std::getline(file,line)) {
		if (! line.length()) continue;
		if (line[0] == '#') continue;
		if (line[0] == ';') continue;

		line=noupper(line);
		if (line[0] == '[') {
			inSection=trim(line.substr(1,line.find(']')-1));
			continue;
		}

		posEqual=line.find('=');
		name  = trim(line.substr(0,posEqual));
		value = trim(line.substr(posEqual+1));

		content_[inSection+'/'+name]=value;
	}
}

int ConfigFile::set(string &dst, string const& section, string const& entry) const {

	map<string,string>::const_iterator ci = content_.find(section + '/' + entry);

	if (ci == content_.end()){
		log("segget.conf has no settings for "+entry+" in ["+section+"] section. Will be set to default:"+dst);
		return 1;
	}
	else{
		dst=ci->second;
		log("Settings: ["+section+"]."+entry+"="+dst);
		return 0;
	}
}
int ConfigFile::set(uint &dst, string const& section, string const& entry) const {

	map<string,string>::const_iterator ci = content_.find(section + '/' + entry);

	if (ci == content_.end()){
		log("segget.conf has no settings for "+entry+" in ["+section+"] section. Will be set to default:"+toString(dst));
		return 1;
	}
	else{
		dst=atoi(ci->second.c_str());
		log("Settings: ["+section+"]."+entry+"="+toString(dst));
		return 0;
	}
}
int ConfigFile::set(bool &dst, string const& section, string const& entry) const {

	map<std::string,string>::const_iterator ci = content_.find(section + '/' + entry);

	if (ci == content_.end()){
		log("segget.conf has no settings for "+entry+" in ["+section+"] section. Will be set to default:"+toString(dst));
		return 1;
	}
	else{
		dst=atoi(ci->second.c_str());
		log("Settings: ["+section+"]."+entry+"="+toString(dst));
		return 0;
	}
}