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

#include "settings.h"

Tsettings settings;

void Tsettings::init(){
	try{
		Tconfig conf("/etc/seggetd/tuiclient.conf");
		conf.set("logs","general_log_file",general_log_file);
		conf.set("logs","logs_dir",logs_dir);
		conf.set("logs","error_log_file",error_log_file);
		conf.set("logs","debug_log_file",debug_log_file);

		conf.set_color("colors","color_distfile_added_connected_fg",color_distfile_added_connected_fg);
		conf.set_color("colors","color_distfile_added_connected_bg",color_distfile_added_connected_bg);
		conf.set_color("colors","color_distfile_added_disconnected_fg",color_distfile_added_disconnected_fg);
		conf.set_color("colors","color_distfile_added_disconnected_bg",color_distfile_added_disconnected_bg);

		conf.set_color("colors","color_distfile_waiting_connected_fg",color_distfile_waiting_connected_fg);
		conf.set_color("colors","color_distfile_waiting_connected_bg",color_distfile_waiting_connected_bg);
		conf.set_color("colors","color_distfile_waiting_disconnected_fg",color_distfile_waiting_disconnected_fg);
		conf.set_color("colors","color_distfile_waiting_disconnected_bg",color_distfile_waiting_disconnected_bg);

		conf.set_color("colors","color_distfile_script_rejected_connected_fg",color_distfile_script_rejected_connected_fg);
		conf.set_color("colors","color_distfile_script_rejected_connected_bg",color_distfile_script_rejected_connected_bg);
		conf.set_color("colors","color_distfile_script_rejected_disconnected_fg",color_distfile_script_rejected_disconnected_fg);
		conf.set_color("colors","color_distfile_script_rejected_disconnected_bg",color_distfile_script_rejected_disconnected_bg);

		conf.set_color("colors","color_distfile_downloading_connected_fg",color_distfile_downloading_connected_fg);
		conf.set_color("colors","color_distfile_downloading_connected_bg",color_distfile_downloading_connected_bg);
		conf.set_color("colors","color_distfile_downloading_disconnected_fg",color_distfile_downloading_disconnected_fg);
		conf.set_color("colors","color_distfile_downloading_disconnected_bg",color_distfile_downloading_disconnected_bg);

		conf.set_color("colors","color_distfile_downloaded_connected_fg",color_distfile_downloaded_connected_fg);
		conf.set_color("colors","color_distfile_downloaded_connected_bg",color_distfile_downloaded_connected_bg);
		conf.set_color("colors","color_distfile_downloaded_disconnected_fg",color_distfile_downloaded_disconnected_fg);
		conf.set_color("colors","color_distfile_downloaded_disconnected_bg",color_distfile_downloaded_disconnected_bg);

		conf.set_color("colors","color_distfile_failed_connected_fg",color_distfile_failed_connected_fg);
		conf.set_color("colors","color_distfile_failed_connected_bg",color_distfile_failed_connected_bg);
		conf.set_color("colors","color_distfile_failed_disconnected_fg",color_distfile_failed_disconnected_fg);
		conf.set_color("colors","color_distfile_failed_disconnected_bg",color_distfile_failed_disconnected_bg);

		conf.set_color("colors","color_distfiles_window_connected_fg",color_distfiles_window_connected_fg);
		conf.set_color("colors","color_distfiles_window_connected_bg",color_distfiles_window_connected_bg);
		conf.set_color("colors","color_distfiles_window_disconnected_fg",color_distfiles_window_disconnected_fg);
		conf.set_color("colors","color_distfiles_window_disconnected_bg",color_distfiles_window_disconnected_bg);

		conf.set_color("colors","color_scroll_window_connected_fg",color_scroll_window_connected_fg);
		conf.set_color("colors","color_scroll_window_connected_bg",color_scroll_window_connected_bg);
		conf.set_color("colors","color_scroll_window_disconnected_fg",color_scroll_window_disconnected_fg);
		conf.set_color("colors","color_scroll_window_disconnected_bg",color_scroll_window_disconnected_bg);

		conf.set_color("colors","color_downloads_connected_fg",color_downloads_connected_fg);
		conf.set_color("colors","color_downloads_connected_bg",color_downloads_connected_bg);
		conf.set_color("colors","color_downloads_disconnected_fg",color_downloads_disconnected_fg);
		conf.set_color("colors","color_downloads_disconnected_bg",color_downloads_disconnected_bg);

		conf.set_color("colors","color_info_connected_fg",color_info_connected_fg);
		conf.set_color("colors","color_info_connected_bg",color_info_connected_bg);
		conf.set_color("colors","color_info_disconnected_fg",color_info_disconnected_fg);
		conf.set_color("colors","color_info_disconnected_bg",color_info_disconnected_bg);

		conf.set_color("colors","color_status_connected_fg",color_status_connected_fg);
		conf.set_color("colors","color_status_connected_bg",color_status_connected_bg);
		conf.set_color("colors","color_status_disconnected_fg",color_status_disconnected_fg);
		conf.set_color("colors","color_status_disconnected_bg",color_status_disconnected_bg);

		conf.set("ui_server","ui_ip",ui_ip);
		conf.set("ui_server","ui_port",ui_port,1,65535);

		conf.clear();
	}catch(...){
//		error_log_no_msg("Error in settings.cpp: init()");
	}
}