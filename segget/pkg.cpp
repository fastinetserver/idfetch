/*
* Copyright (C) 2010 Robin H.Johnson, Ovechko Kostyantyn <fastinetserver@gmail.com>.
*
* Project: IDFetch.
* Developer: Ovechko Kostyantyn Olexandrovich (Kharkiv State Technical University of Construction and Architecture, Ukraine).
* Mentor: Robin H. Johnson (Gentoo Linux: Developer, Trustee & Infrastructure Lead).
* Mentoring organization: Gentoo Linux.
* Sponsored by GSOC 2010.
*
* This file is part of Segget.
*
* Segget is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* Segget is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with Segget; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "pkg.h"

void Tpkg::push_back_distfile(json_object* json_distfile){
	try{
		distfile_count++;
		Tdistfile * Pcur_distfile=new Tdistfile();
		Pdistfile_list.push_back(Pcur_distfile);
		Pcur_distfile->load_distfile_from_json(json_distfile);
	}catch(...){
		error_log("Error in pkg.cpp: pushback_distfile()");
	}
}


void Tpkg::load_distfile_list(json_object* json_array_distfile_list){
	try{
		ulong distfile_counter=json_object_array_length(json_array_distfile_list);
		for(uint distfile_array_item_num=0;distfile_array_item_num<distfile_counter;distfile_array_item_num++){
			push_back_distfile(json_object_array_get_idx(json_array_distfile_list,distfile_array_item_num));
		}
	}catch(...){
		error_log("Error in pkg.cpp: load_distfile_list()");
	}
}
/*
void Tpkg::load_distfile_list(json_object* json_array_distfile_list){
	try{
		distfile_count=json_object_array_length(json_array_distfile_list);
//		Pdistfile_list= new Tdistfile* [distfile_count];
		for(uint distfile_array_item_num=0;distfile_array_item_num<distfile_count;distfile_array_item_num++){
//			Pdistfile_list[distfile_array_item_num]=new Tdistfile();
			Pdistfile_list.push_back(new Tdistfile());
//			Tdistfile a_distfile;
//			a_distfile.load_distfile_from_json(json_object_array_get_idx(json_array_distfile_list,distfile_array_item_num));
			Pdistfile_list[distfile_array_item_num]->load_distfile_from_json(json_object_array_get_idx(json_array_distfile_list,distfile_array_item_num));
		}
	}catch(...){
		error_log("Error in pkg.cpp: load_distfile_list()");
	}
}
*/



void Tpkg::load_pkg_from_json(json_object* json_obj_pkg){
	try{
		//	printf("\t%d %s\n",array_item_num, json_object_to_json_string(json_obj_pkg));
		name=json_object_to_json_string(json_object_object_get(json_obj_pkg,"pkg_name"));
		category=json_object_to_json_string(json_object_object_get(json_obj_pkg,"CATEGORY"));
		load_distfile_list(json_object_object_get(json_obj_pkg,"distfile_list"));
	}catch(...){
		error_log("Error in pkg.cpp: load_pkg_from_json()");
	}
}

Tpkg::~Tpkg(){
	try{
		for (uint i=0;i<distfile_count;i++)
			delete Pdistfile_list[i];
		Pdistfile_list.clear();
//		delete [] Pdistfile_list;
	}catch(...){
		error_log("Error in pkg.cpp: ~Tpkg");
	}
}