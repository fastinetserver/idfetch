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

#include "helpwindow.h"

void Thelp_window::compose(){
//		msg_text=msg_text+" ";
//	
	make_frame();
	mvwaddstr(window,2,1,"q      - quit");
	mvwaddstr(window,3,1,"h      - show this help");
	mvwaddstr(window,4,1,"l      - show log");
	mvwaddstr(window,5,1,"e      - show error log");
	mvwaddstr(window,6,1,"d      - show distfiles queue");
	mvwaddstr(window,7,1,"Up     - scroll 1 line up");
	mvwaddstr(window,8,1,"Down   - scroll 1 line down");
	mvwaddstr(window,9,1,"PgUp   - scroll 1 page up");
	mvwaddstr(window,10,1,"PgDown - scroll 1 page down");
	wrefresh(window);
}