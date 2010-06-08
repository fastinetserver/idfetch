#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "json/json.h"
#include "distfile.cpp"

using namespace std;

class Tpkg{
	public:
		Tdistfile **Pdistfile_list;
		string name;
		string category;
		int distfile_count;
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