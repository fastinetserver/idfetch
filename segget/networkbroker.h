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
#ifndef __NETWORK_DISTFILE_BROKER_H__
#define __NETWORK_DISTFILE_BROKER_H__

#include <vector>
#include <sys/time.h>
#include "shorttypes.h"
#include "response.h"
#include "phase.h"
using namespace std;

class Tnetwork_distfile_broker{
	public:
//		map<string,Tmirror *> benchmarked_mirror_list;
		vector<bool> mirror_fails_vector;
		timeval last_request_time;
		ulong network_num;
		ulong failed_mirrors_num;
		Tnetwork_distfile_broker_phases phase;
		uint proxy_fetcher_response;
		Tnetwork_distfile_broker():
			mirror_fails_vector(),
			last_request_time(),
			network_num(0),
			failed_mirrors_num(0),
			phase(E_USE_AS_LOCAL_MIRRORS),
			proxy_fetcher_response(R_PF_NOT_REQUESTED_YET)
			{};
		void init(ulong network_number);
//		void mirror_failed(uint mirror_num);
		void local_mirror_failed(uint mirror_num);
		void proxy_fetcher_mirror_failed(uint mirror_num);
//		bool some_mirrors_have_NOT_failed_yet();
};

#endif
