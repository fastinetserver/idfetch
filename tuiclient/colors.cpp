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

#include "colors.h"

void color_distfile_added(WINDOW * window){
	if(has_colors()){
		start_color();			/* Start color 			*/
		wattron(window, COLOR_PAIR(4));
	}
}

void color_distfile_waiting(WINDOW * window){
	if(has_colors()){
		start_color();			/* Start color 			*/
		wattron(window, COLOR_PAIR(8));
	}
}

void color_distfile_downloading(WINDOW * window){
	if(has_colors()){
		start_color();			/* Start color 			*/
		wattron(window, COLOR_PAIR(6));
	}
}

void color_distfile_downloaded(WINDOW * window){
	if(has_colors()){
		start_color();			/* Start color 			*/
		wattron(window, COLOR_PAIR(5));
	}
}

void color_distfile_failed(WINDOW * window){
	if(has_colors()){
		start_color();			/* Start color 			*/
		wattron(window, COLOR_PAIR(7));
	}
}

void color_distfiles_window(WINDOW * window){
	if(has_colors()){
		start_color();			/* Start color 			*/
		wattron(window, COLOR_PAIR(4));
	}
}

void color_scroll_window(WINDOW * window){
	if(has_colors()){
		start_color();			/* Start color 			*/
		wbkgd(window, COLOR_PAIR(5));
		wattron(window, COLOR_PAIR(5));
	}
}

void color_downloads(WINDOW * window){
	if(has_colors()){
		start_color();			/* Start color 			*/
//		wbkgd(window, COLOR_PAIR(1));
		wattron(window, COLOR_PAIR(1));
	}
}

void color_info(WINDOW * window){
	if(has_colors()){
		start_color();			/* Start color 			*/
//		wbkgd(window, COLOR_PAIR(2));
		wattron(window,COLOR_PAIR(2));
	}
}

void color_status(WINDOW * window){
	if(has_colors()){
		start_color();			/* Start color 			*/
//		wbkgd(window, COLOR_PAIR(3));
		wattron(window,COLOR_PAIR(3));
	}
}

void colors_connected(){
	if(has_colors()){
		start_color();			/* Start color 			*/
		init_color(COLOR_BLUE, 300, 300, 300); 
		init_pair(1, COLOR_WHITE, COLOR_BLUE);
		init_pair(2, COLOR_BLACK, COLOR_CYAN);
		init_pair(3, COLOR_GREEN, COLOR_BLUE);
		init_pair(4, COLOR_WHITE, COLOR_BLACK);
		init_pair(5, COLOR_GREEN, COLOR_BLACK);
		init_pair(6, COLOR_BLUE,  COLOR_BLACK);
		init_pair(7, COLOR_RED,   COLOR_BLACK);
		init_pair(8, COLOR_YELLOW,   COLOR_BLACK);
//		init_pair(4, COLOR_BLACK, COLOR_WHITE);
//		attrset(A_REVERSE);
		attrset(A_DIM);
	}
}

void colors_disconnected(){
	if(has_colors()){
		start_color();			/* Start color 			*/
		init_color(COLOR_BLUE, 400, 400, 400); 
		init_pair(1, COLOR_BLACK, COLOR_WHITE);
		init_pair(2, COLOR_WHITE, COLOR_BLACK);
		init_pair(3, COLOR_BLACK, COLOR_RED);
		init_pair(4, COLOR_WHITE, COLOR_BLACK);
		init_pair(5, COLOR_WHITE, COLOR_BLACK);
		init_pair(6, COLOR_WHITE, COLOR_BLACK);
		init_pair(7, COLOR_WHITE, COLOR_BLACK);
		init_pair(8, COLOR_WHITE, COLOR_BLACK);
///		attrset(A_BOLD);
//		attrset(A_REVERSE);
	}
}