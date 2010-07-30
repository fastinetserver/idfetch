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

#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include "tui.h"

using namespace std;

#define R_NOT_SET										300
#define R_LM_WAIT_FOR_LOCAL_MIRRORS						50

#define R_PF_NOT_REQUESTED_YET							100
#define R_PF_ADDED_TO_PROXY_QUEUE						101
#define R_PF_ALREADY_WAS_IN_QUEUE						102
#define R_PF_DOWNLOADED									103
#define R_PF_BE_MORE_PATIENT							104
#define R_PF_ERROR_ADDING_TO_PROXY_QUEUE				105
#define R_PF_FAILED										106
#define R_PF_REJECTED									107

// 0 for succesfull return of provide_segment()
#define R_R_DOWNLOAD_STARTED							0
#define R_R_WAITING										108
#define R_R_DOWNLOADING									109

#define R_LM_PF_R_NO_FREE_NETWORK_CONNECTION_FOUND		110

#define ALLOW_REQUESTS_TO_PROXY_FETCHERS				201
#define DO_NOT_ALLOW_REQUESTS_TO_PROXY_FETCHERS			202
#define ALLOW_REMOTE_NETWORKS							203
#define DO_NOT_ALLOW_REMOTE_NETWORKS					204
#define ALLOW_LOWER_PRIORITY_NETWORKS					205

#define REJECTED_BY_USER_PYTHON_SCRIPT					301
#define ERROR_WHILE_PREPARING_CONNECTION				302

int decode_server_response(string server_response);

#endif