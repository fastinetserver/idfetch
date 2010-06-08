#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "json/json.h"
#include "pkg.cpp"

using namespace std;

int pkg_count;
Tpkg **Ppkg_array;

void load_pkgs(){
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

void show_pkgs(){
	for (int array_item_num=0;array_item_num<pkg_count;array_item_num++){
		cout <<"PKG:"<<array_item_num<<") cat:"<< Ppkg_array[array_item_num]->category <<" name:"<< Ppkg_array[array_item_num]->name <<"\n";
		for(int distfile_array_item_num=0;distfile_array_item_num<Ppkg_array[array_item_num]->distfile_count;distfile_array_item_num++){
			cout << "    "<< distfile_array_item_num<<") distfile_name:"<< Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->name<<"\n";
			for(int urllist_array_item_num=0;urllist_array_item_num<Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->url_count;urllist_array_item_num++){
				cout <<"        "<<Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->url_list[urllist_array_item_num]<<"\n";
			}
		}
	}
};


int main()
{
  load_pkgs();
  show_pkgs();

  int array_item_num=0;
  int distfile_array_item_num=0;
  for (int array_item_num=0;array_item_num<pkg_count;array_item_num++){
    //cout <<"PKG:"<<array_item_num<<") cat:"<< Ppkg_array[array_item_num]->category <<" name:"<< Ppkg_array[array_item_num]->name <<"\n";
    for(int distfile_array_item_num=0;distfile_array_item_num<Ppkg_array[array_item_num]->distfile_count;distfile_array_item_num++){
      //cout << "    "<< distfile_array_item_num<<") distfile_name:"<< Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->show_name() <<"\n";
      for(int urllist_array_item_num=0;urllist_array_item_num<Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->url_count;urllist_array_item_num++){
	//				cout <<"        "<<Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->url_list[urllist_array_item_num]<<"\n";
	if (Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->url_count)
	  Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->dosegments();
      }
    }
  }
  //      Ppkg_array[array_item_num]->Pdistfile_list[distfile_array_item_num]->dosegments();
  
  return 0;
}
