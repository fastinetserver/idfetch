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

#ifndef __STATS_H__
#define __STATS_H__

#include <sys/time.h>
#include "connection.h"
#include "tui.h"
#include "settings.h"
using namespace std;

class Tstats{
	private:
		ulong dld_size;
		ulong dld_distfiles_count;
		uint total_size;
	public:
		ulong total_bytes_per_last_interval;
		struct timeval previous_time;
		double last_time_interval;
		ulong avg_total_speed;
		uint pkg_count;
		ulong distfiles_count;
		ulong segments_count;
		ulong dld_segments_count;
		ulong active_connections_counter;
		Tstats():
			dld_size(0),
			dld_distfiles_count(0),
			total_size(0),
			total_bytes_per_last_interval(0),
			previous_time(),
			last_time_interval(1),
			avg_total_speed(0),
			pkg_count(0),
			distfiles_count(0),
			segments_count(0),
			dld_segments_count(0),
			active_connections_counter(0)
			{};
		void inc_dld_size(ulong more_bytes){ dld_size+=more_bytes;};
		ulong get_dld_size(){return dld_size;};
		void inc_dld_distfiles_count();
		ulong get_dld_distfiles_count(){return dld_distfiles_count;};
		void inc_total_size(ulong more_bytes){ total_size+=more_bytes;};
		ulong get_total_size(){return total_size;};
		void show_totals();
		void reset_previous_time();
};

extern Tstats stats;

#endif