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

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <string>
#include "config.h"

//#include "tui.cpp"

using namespace std;

class Tsettings{
	private:
	public:
	// cli arguments
		string arg_wait_distfiles;
		vector <string> wait_distfiles_vector;
	//ui_server
		string ui_ip;
		ulong ui_port;
	//colors
		int color_distfile_added_connected_fg;
		int color_distfile_added_connected_bg;
		int color_distfile_added_disconnected_fg;
		int color_distfile_added_disconnected_bg;

		int color_distfile_waiting_connected_fg;
		int color_distfile_waiting_connected_bg;
		int color_distfile_waiting_disconnected_fg;
		int color_distfile_waiting_disconnected_bg;

		int color_distfile_script_rejected_connected_fg;
		int color_distfile_script_rejected_connected_bg;
		int color_distfile_script_rejected_disconnected_fg;
		int color_distfile_script_rejected_disconnected_bg;

		int color_distfile_downloading_connected_fg;
		int color_distfile_downloading_connected_bg;
		int color_distfile_downloading_disconnected_fg;
		int color_distfile_downloading_disconnected_bg;

		int color_distfile_downloaded_connected_fg;
		int color_distfile_downloaded_connected_bg;
		int color_distfile_downloaded_disconnected_fg;
		int color_distfile_downloaded_disconnected_bg;

		int color_distfile_failed_connected_fg;
		int color_distfile_failed_connected_bg;
		int color_distfile_failed_disconnected_fg;
		int color_distfile_failed_disconnected_bg;

		int color_distfiles_window_connected_fg;
		int color_distfiles_window_connected_bg;
		int color_distfiles_window_disconnected_fg;
		int color_distfiles_window_disconnected_bg;

		int color_scroll_window_connected_fg;
		int color_scroll_window_connected_bg;
		int color_scroll_window_disconnected_fg;
		int color_scroll_window_disconnected_bg;

		int color_downloads_connected_fg;
		int color_downloads_connected_bg;
		int color_downloads_disconnected_fg;
		int color_downloads_disconnected_bg;

		int color_info_connected_fg;
		int color_info_connected_bg;
		int color_info_disconnected_fg;
		int color_info_disconnected_bg;

		int color_status_connected_fg;
		int color_status_connected_bg;
		int color_status_disconnected_fg;
		int color_status_disconnected_bg;
	//logs
		string logs_dir;
		string general_log_file;
		string error_log_file;
		string debug_log_file;

		Tsettings():
		// cli arguments
			arg_wait_distfiles(""),
			wait_distfiles_vector(),
		//ui_server
			ui_ip("127.0.0.1"),
			ui_port(9999),
		//colors
			color_distfile_added_connected_fg(COLOR_WHITE),
			color_distfile_added_connected_bg(COLOR_BLACK),
			color_distfile_added_disconnected_fg(COLOR_WHITE),
			color_distfile_added_disconnected_bg(COLOR_BLACK),

			color_distfile_waiting_connected_fg(COLOR_BLUE),
			color_distfile_waiting_connected_bg(COLOR_BLACK),
			color_distfile_waiting_disconnected_fg(COLOR_WHITE),
			color_distfile_waiting_disconnected_bg(COLOR_BLACK),

			color_distfile_script_rejected_connected_fg(COLOR_YELLOW),
			color_distfile_script_rejected_connected_bg(COLOR_BLACK),
			color_distfile_script_rejected_disconnected_fg(COLOR_WHITE),
			color_distfile_script_rejected_disconnected_bg(COLOR_BLACK),

			color_distfile_downloading_connected_fg(COLOR_CYAN),
			color_distfile_downloading_connected_bg(COLOR_BLACK),
			color_distfile_downloading_disconnected_fg(COLOR_WHITE),
			color_distfile_downloading_disconnected_bg(COLOR_BLACK),

			color_distfile_downloaded_connected_fg(COLOR_GREEN),
			color_distfile_downloaded_connected_bg(COLOR_BLACK),
			color_distfile_downloaded_disconnected_fg(COLOR_WHITE),
			color_distfile_downloaded_disconnected_bg(COLOR_BLACK),

			color_distfile_failed_connected_fg(COLOR_RED),
			color_distfile_failed_connected_bg(COLOR_BLACK),
			color_distfile_failed_disconnected_fg(COLOR_WHITE),
			color_distfile_failed_disconnected_bg(COLOR_BLACK),

			color_distfiles_window_connected_fg(COLOR_WHITE),
			color_distfiles_window_connected_bg(COLOR_BLACK),
			color_distfiles_window_disconnected_fg(COLOR_WHITE),
			color_distfiles_window_disconnected_bg(COLOR_BLACK),

			color_scroll_window_connected_fg(COLOR_GREEN),
			color_scroll_window_connected_bg(COLOR_BLACK),
			color_scroll_window_disconnected_fg(COLOR_WHITE),
			color_scroll_window_disconnected_bg(COLOR_BLACK),

			color_downloads_connected_fg(COLOR_WHITE),
			color_downloads_connected_bg(COLOR_BLACK),
			color_downloads_disconnected_fg(COLOR_BLACK),
			color_downloads_disconnected_bg(COLOR_WHITE),

			color_info_connected_fg(COLOR_BLACK),
			color_info_connected_bg(COLOR_CYAN),
			color_info_disconnected_fg(COLOR_WHITE),
			color_info_disconnected_bg(COLOR_BLACK),

			color_status_connected_fg(COLOR_GREEN),
			color_status_connected_bg(COLOR_BLACK),
			color_status_disconnected_fg(COLOR_BLACK),
			color_status_disconnected_bg(COLOR_RED),

		//logs
			logs_dir("./logs"),
			general_log_file("tuiclient.log"),
			error_log_file("error.log"),
			debug_log_file("debug.log")
			{};
		void init();
};

extern Tsettings settings;
#endif