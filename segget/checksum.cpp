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

#include "checksum.h"

bool rmd160_ok(string distfile_filename, string portage_hash){
	try{
		if (portage_hash=="") return true;
		std::string digest;
		CryptoPP::RIPEMD160 hash;
		CryptoPP::FileSource distfile_to_hash(distfile_filename.c_str(), true,
			new CryptoPP::HashFilter(hash,
				new CryptoPP::HexEncoder(
					new CryptoPP::StringSink(digest))));
		if (portage_hash==noupper(digest)){
			log("RMD160 checksum for distfile:"+distfile_filename+" is [OK]");
			return true;
		}else{
			log("Error: RMD160 checksum for distfile:"+distfile_filename+" [FAILED]");
			error_log("Error: RMD160 checksum for distfile:"+distfile_filename+" [FAILED]");
			return false;
		}
	}catch(...){
		error_log("Error in checksum.cpp : rmd160_ok()");
		return false;
	}
}
bool sha1_ok(string distfile_filename, string portage_hash){
	try{
		if (portage_hash=="") return true;
		std::string digest;
		CryptoPP::SHA1 hash;
		CryptoPP::FileSource distfile_to_hash(distfile_filename.c_str(), true,
			new CryptoPP::HashFilter(hash,
				new CryptoPP::HexEncoder(
					new CryptoPP::StringSink(digest))));
//		debug("SHA1:"+noupper(digest));
		if (portage_hash==noupper(digest)){
			log("SHA1 checksum for distfile:"+distfile_filename+" is [OK]");
			return true;
		}else{
			log("Error: SHA1 checksum for distfile:"+distfile_filename+" [FAILED]");
			error_log("Error: SHA1 checksum for distfile:"+distfile_filename+" [FAILED]");
			return false;
		}
	}catch(...){
		error_log("Error in checksum.cpp : sha1_ok()");
		return false;
	}
}
bool sha256_ok(string distfile_filename, string portage_hash){
	try{
		if (portage_hash=="") return true;
		std::string digest;
		CryptoPP::SHA256 hash;
		CryptoPP::FileSource distfile_to_hash(distfile_filename.c_str(), true,
			new CryptoPP::HashFilter(hash,
				new CryptoPP::HexEncoder(
					new CryptoPP::StringSink(digest))));
//		debug("SHA256:"+noupper(digest));
		if (portage_hash==noupper(digest)){
			log("SHA256 checksum for distfile:"+distfile_filename+" is [OK]");
			return true;
		}else{
			log("Error: SHA256 checksum for distfile:"+distfile_filename+" [FAILED]");
			error_log("Error: SHA256 checksum for distfile:"+distfile_filename+" [FAILED]");
			return false;
		}
	}catch(...){
		error_log("Error in checksum.cpp : sha256_ok()");
		return false;
	}
}
bool sha512_ok(string distfile_filename, string portage_hash){
	try{
		if (portage_hash=="") return true;
		std::string digest;
		CryptoPP::SHA512 hash;
		CryptoPP::FileSource distfile_to_hash(distfile_filename.c_str(), true,
			new CryptoPP::HashFilter(hash,
				new CryptoPP::HexEncoder(
					new CryptoPP::StringSink(digest))));
//		debug("SHA256:"+noupper(digest));
		if (portage_hash==noupper(digest)){
			log("SHA512 checksum for distfile:"+distfile_filename+" is [OK]");
			return true;
		}else{
			log("Error: SHA512 checksum for distfile:"+distfile_filename+" [FAILED]");
			error_log("Error: SHA512 checksum for distfile:"+distfile_filename+" [FAILED]");
			return false;
		}
	}catch(...){
		error_log("Error in checksum.cpp : sha512_ok()");
		return false;
	}
}
bool whirlpool_ok(string distfile_filename, string portage_hash){
	try{
		if (portage_hash=="") return true;
		std::string digest;
		CryptoPP::Whirlpool hash;
		CryptoPP::FileSource distfile_to_hash(distfile_filename.c_str(), true,
			new CryptoPP::HashFilter(hash,
				new CryptoPP::HexEncoder(
					new CryptoPP::StringSink(digest))));
//		debug("SHA256:"+noupper(digest));
		if (portage_hash==noupper(digest)){
			log("WHIRLPOOL checksum for distfile:"+distfile_filename+" is [OK]");
			return true;
		}else{
			log("Error: WHIRLPOOL checksum for distfile:"+distfile_filename+" [FAILED]");
			error_log("Error: WHIRLPOOL checksum for distfile:"+distfile_filename+" [FAILED]");
			return false;
		}
	}catch(...){
		error_log("Error in checksum.cpp : whirlpool_ok()");
		return false;
	}
}
bool md5_ok(string distfile_filename, string portage_hash){
	try{
		if (portage_hash=="") return true;
		std::string digest;
		CryptoPP::Weak::MD5 hash;
		CryptoPP::FileSource distfile_to_hash(distfile_filename.c_str(), true,
			new CryptoPP::HashFilter(hash,
				new CryptoPP::HexEncoder(
					new CryptoPP::StringSink(digest))));
//		debug("SHA256:"+noupper(digest));
		if (portage_hash==noupper(digest)){
			log("MD5 checksum for distfile:"+distfile_filename+" is [OK]");
			return true;
		}else{
			log("Error: MD5 checksum for distfile:"+distfile_filename+" [FAILED]");
			error_log("Error: MD5 checksum for distfile:"+distfile_filename+" [FAILED]");
			return false;
		}
	}catch(...){
		error_log("Error in checksum.cpp : md5_ok()");
		return false;
	}
}
bool crc32_ok(string distfile_filename, string portage_hash){
	try{
		if (portage_hash=="") return true;
		std::string digest;
		CryptoPP::CRC32 hash;
		CryptoPP::FileSource distfile_to_hash(distfile_filename.c_str(), true,
			new CryptoPP::HashFilter(hash,
				new CryptoPP::HexEncoder(
					new CryptoPP::StringSink(digest))));
//		debug("SHA256:"+noupper(digest));
		if (portage_hash==noupper(digest)){
			log("CRC32 checksum for distfile:"+distfile_filename+" is [OK]");
			return true;
		}else{
			log("Error: CRC32 checksum for distfile:"+distfile_filename+" [FAILED]");
			error_log("Error: CRC32 checksum for distfile:"+distfile_filename+" [FAILED]");
			return false;
		}
	}catch(...){
		error_log("Error in checksum.cpp : crc32_ok()");
		return false;
	}
}