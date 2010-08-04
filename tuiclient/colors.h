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
* modify it under t	color_downloads(window);
he terms of the GNU Lesser General Public
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

#ifndef _COLORS_H_
#define _COLORS_H_

#include <ncurses.h>

void color_distfile_added(WINDOW * window);
void color_distfile_waiting(WINDOW * window);
void color_distfile_downloading(WINDOW * window);
void color_distfile_downloaded(WINDOW * window);
void color_distfile_failed(WINDOW * window);
void color_distfiles_window(WINDOW * window);
void color_scroll_window(WINDOW * window);
void color_downloads(WINDOW * window);
void color_info(WINDOW * window);
void color_status(WINDOW * window);
void colors_connected();
void colors_disconnected();
#endif