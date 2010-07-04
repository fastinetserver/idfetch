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

#ifndef __CHECKSUM_H__
#define __CHECKSUM_H__

#include <string>
#include "cryptopp/crc.h"
#include "cryptopp/sha.h"
#include "cryptopp/whrlpool.h"
#include "cryptopp/ripemd.h"
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "cryptopp/md5.h"
#include "cryptopp/files.h"
#include "cryptopp/hex.h"
#include "tui.h"
#include "str.h"

bool rmd160_ok(string distfile_filename, string portage_hash);
bool sha1_ok(string distfile_filename, string portage_hash);
bool sha256_ok(string distfile_filename, string portage_hash);
bool sha512_ok(string distfile_filename, string portage_hash);
bool whirlpool_ok(string distfile_filename, string portage_hash);
bool md5_ok(string distfile_filename, string portage_hash);
bool crc32_ok(string distfile_filename, string portage_hash);
#endif