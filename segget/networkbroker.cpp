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

#include "networkbroker.h"
void Tnetwork_distfile_broker::init(ulong network_number){
	network_num=network_number;
	if (network_array[network_num].use_own_mirror_list_only_on){
		// create flags for each mirror from this network
		mirror_fails_vector.insert(mirror_fails_vector.begin(),network_array[network_num].benchmarked_mirror_list.size(),false);
	}
}

bool Tnetwork_distfile_broker::have_all_mirrors_failed(){
	debug("        get_allowed_status_for_network_num="+toString(network_num));
	return network_array[network_num].has_free_connections();
}