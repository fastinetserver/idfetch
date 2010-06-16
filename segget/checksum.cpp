#include <string>
//#include <iostream>
//#include "tui.cpp"
#include "cryptopp/sha.h"
#include "cryptopp/ripemd.h"
#include "cryptopp/files.h"
//#include "cryptopp/base64.h"
//#include "cryptopp/base32.h"
#include "cryptopp/hex.h"
#include "tui.cpp"

int lower_char(int c)
{
  return std::tolower((unsigned char)c);
}

std::string noupper(string s){ 
	std::transform(s.begin(), s.end(), s.begin(), lower_char);
	return s; 
} 

bool rmd160_ok(string distfile_filename, string portage_hash){
	std::string digest;
	CryptoPP::RIPEMD160 hash;  // don't use MD5 anymore. It is considered insecure
	CryptoPP::FileSource distfile_to_hash(distfile_filename.c_str(), true,
		new CryptoPP::HashFilter(hash,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(digest))));
	if (portage_hash==noupper(digest))
		return true;
	else
		return false;
}
bool sha1_ok(string distfile_filename, string portage_hash){
	std::string digest;
	CryptoPP::SHA1 hash;  // don't use MD5 anymore. It is considered insecure
	CryptoPP::FileSource distfile_to_hash(distfile_filename.c_str(), true,
		new CryptoPP::HashFilter(hash,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(digest))));
//	debug("SHA1:"+noupper(digest));
	if (portage_hash==noupper(digest))
		return true;
	else
		return false;
}
bool sha256_ok(string distfile_filename, string portage_hash){
	std::string digest;
	CryptoPP::SHA256 hash;  // don't use MD5 anymore. It is considered insecure
	CryptoPP::FileSource distfile_to_hash(distfile_filename.c_str(), true,
		new CryptoPP::HashFilter(hash,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(digest))));
//	debug("SHA256:"+noupper(digest));
	if (portage_hash==noupper(digest))
		return true;
	else
		return false;
}