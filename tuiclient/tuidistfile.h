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

#ifndef __TUIDISTFILE_H__
#define __TUIDISTFILE_H__
#include <string>
#include <vector>

using namespace std;

enum Tdistfile_status{
	DNEW,
	D_NOT_PROXY_REQUESTED,
	DPROXY_REJECTED,
	DPROXY_QUEUED,
	DPROXY_DOWNLOADING,
	DPROXY_DOWNLOADED,
	DPROXY_FAILED,
	DWAITING,
	DDOWNLOADING,
	DDOWNLOADED,
	DFAILED
};

class Ttuidistfile{
	public:
		string name;
		ulong size;
		Tdistfile_status status;
		ulong dld_bytes;
		ulong dld_segments;
		ulong segments_count;
		Ttuidistfile():
			name(""),
			size(0),
			status(DNEW),
			dld_bytes(0),
			dld_segments(0),
			segments_count(0)
			{};
		~Ttuidistfile(){};
		string statusToString();
};

vector <Ttuidistfile> tuidistfiles;
//vector <string> tuidistfiles;
//string tuidistfiles;

#endif
