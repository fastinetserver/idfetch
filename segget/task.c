#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "json/json.h"


using namespace std;
inline string fix_url(string url);

class Tdistfile{
	public:
		string *url_list;
		string name;
		int size;
		string RMD160;
		string SHA1;
		string SHA256;
		int url_count;
		void set_name(string distfile_name);
		void set_size(int distfile_size);
		string show_name(){return name;};
		int show_size(){return size;};
		void load_url_list(json_object* json_array_distfile_urllist);
		Tdistfile(json_object* json_obj_distfile);
		~Tdistfile();
};

void Tdistfile::load_url_list(json_object* json_array_distfile_urllist){
	url_count=json_object_array_length(json_array_distfile_urllist);
	url_list= new string [url_count];
	for(int urllist_array_item_num=0;urllist_array_item_num<url_count;urllist_array_item_num++)
		url_list[urllist_array_item_num]=fix_url(json_object_to_json_string(json_object_array_get_idx(json_array_distfile_urllist,urllist_array_item_num)));
};


Tdistfile::Tdistfile(json_object* json_obj_distfile){
	name=json_object_to_json_string(json_object_object_get(json_obj_distfile,"name"));
	size=atoi(json_object_to_json_string(json_object_object_get(json_obj_distfile,"size")));
	RMD160=json_object_to_json_string(json_object_object_get(json_obj_distfile,"RMD160"));
	SHA1=json_object_to_json_string(json_object_object_get(json_obj_distfile,"SHA1"));
	SHA256=json_object_to_json_string(json_object_object_get(json_obj_distfile,"SHA256"));
	load_url_list(json_object_object_get(json_obj_distfile,"url_list"));
};
Tdistfile::~Tdistfile(){

	delete [] url_list;
}

void Tdistfile::set_name(string distfile_name){
	name=distfile_name;
};
void Tdistfile::set_size(int distfile_size){
	size=distfile_size;
};

class Tpkg{
	public:
		Tdistfile **Pdistfile_list;
		string name;
		string category;
		int distfile_count;
		string show_name(){return name;};
		string show_category(){return category;};
		void load_distfile_list(json_object* json_array_distfile_list);
		Tpkg(json_object* json_obj_pkg);
		~Tpkg();
};
void Tpkg::load_distfile_list(json_object* json_array_distfile_list){
	distfile_count=json_object_array_length(json_array_distfile_list);
	Pdistfile_list= new Tdistfile* [distfile_count];
	for(int distfile_array_item_num=0;distfile_array_item_num<distfile_count;distfile_array_item_num++){
		Pdistfile_list[distfile_array_item_num]=new Tdistfile(json_object_array_get_idx(json_array_distfile_list,distfile_array_item_num));
	}
};


Tpkg::Tpkg(json_object* json_obj_pkg){
//	printf("\t%d %s\n",array_item_num, json_object_to_json_string(json_obj_pkg));
	name=json_object_to_json_string(json_object_object_get(json_obj_pkg,"pkg_name"));
	category=json_object_to_json_string(json_object_object_get(json_obj_pkg,"CATEGORY"));
	load_distfile_list(json_object_object_get(json_obj_pkg,"distfile_list"));
};

Tpkg::~Tpkg(){
	for (int i=0;i<distfile_count;i++)
		delete Pdistfile_list[i];
	delete [] Pdistfile_list;
};

class Ttask{
		int pkg_count;
		Tpkg **Ppkg_array;
	public:
		void load_pkgs();
		void show_pkgs();
};

void Ttask::load_pkgs(){
	ifstream myfile("pkg.list");
	string buff((std::istreambuf_iterator<char>(myfile)), std::istreambuf_iterator<char>());
	myfile>>buff;
	myfile.close();
//	cout<< buff<<"\n";
        json_object *json_array_pkg_list;
        json_array_pkg_list = json_tokener_parse(buff.c_str());
        if (is_error(json_array_pkg_list))
		printf("error parsing json: %s\n",json_tokener_errors[-(unsigned long)json_array_pkg_list]);
	else {
		pkg_count=json_object_array_length(json_array_pkg_list);
		Ppkg_array= new Tpkg* [pkg_count];
		for(int array_item_num=0;array_item_num<pkg_count;array_item_num++){
			Ppkg_array[array_item_num]=new Tpkg(json_object_array_get_idx(json_array_pkg_list,array_item_num));
		}
	}
};

void Ttask::show_pkgs(){
	for (int array_item_num=0;array_item_num<pkg_count;array_item_num++){
		cout <<"PKG:"<<array_item_num<<") cat:"<< Ppkg_array[array_item_num]->show_category() <<" name:"<< Ppkg_array[array_item_num]->show_name() <<"\n";
		for(int distfile_array_item_num=0;distfile_array_item_num<Ppkg_array[array_item_num]->distfile_count;distfile_array_item_num++){
			cout << "    "<< distfile_array_item_num<<") distfile_name:"<< Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->show_name() <<"\n";
			for(int urllist_array_item_num=0;urllist_array_item_num<Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->url_count;urllist_array_item_num++){
				cout <<"        "<<Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->url_list[urllist_array_item_num]<<"\n";
			}
		}
	}
};
inline string fix_url(string url){
	string searchString( "\\/" ); 
	string replaceString( "/" );
	string::size_type pos = 0;
	while ( (pos = url.find(searchString, pos)) != string::npos ) {
		url.replace( pos, searchString.size(), replaceString );
		pos++;
	}
	return url;
}
int main()
{
	Ttask* Pmytask=new Ttask;
	Pmytask->load_pkgs();
	Pmytask->show_pkgs();
	delete Pmytask;
        return 0;
}