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

void color_downloads(){
	if(has_colors()){
		start_color();			/* Start color 			*/
		attron(COLOR_PAIR(1));
	}
}

void color_info(){
	if(has_colors()){
		start_color();			/* Start color 			*/
		attron(COLOR_PAIR(2));
	}
}

void color_status(){
	if(has_colors()){
		start_color();			/* Start color 			*/
		attron(COLOR_PAIR(3));
	}
}

void color_disconnected(){
	if(has_colors()){
		start_color();			/* Start color 			*/
		init_pair(1, COLOR_WHITE, COLOR_MAGENTA);
		attron(COLOR_PAIR(1));
	}
}
void color_init(){
	if(has_colors()){
		init_color(COLOR_MAGENTA, 300, 300, 300); 
	}
}
void colors_connected(){
	if(has_colors()){
		start_color();			/* Start color 			*/
		init_color(COLOR_BLUE, 300, 300, 300); 
		init_pair(1, COLOR_WHITE, COLOR_BLUE);
		init_pair(2, COLOR_BLACK, COLOR_CYAN);
		init_pair(3, COLOR_GREEN, COLOR_BLUE);
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
//		attrset(A_BOLD);
//		attrset(A_REVERSE);
	}
}