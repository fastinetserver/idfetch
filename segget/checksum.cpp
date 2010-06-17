#include <string>
#include "cryptopp/crc.h"
#include "cryptopp/sha.h"
#include "cryptopp/whrlpool.h"
#include "cryptopp/ripemd.h"
#include "cryptopp/md5.h"
#include "cryptopp/files.h"
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
	CryptoPP::RIPEMD160 hash;
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
	CryptoPP::SHA1 hash;
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
	CryptoPP::SHA256 hash;
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
bool sha512_ok(string distfile_filename, string portage_hash){
	std::string digest;
	CryptoPP::SHA512 hash;
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
bool whirlpool_ok(string distfile_filename, string portage_hash){
	std::string digest;
	CryptoPP::Whirlpool hash;
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
bool md5_ok(string distfile_filename, string portage_hash){
	std::string digest;
	CryptoPP::MD5 hash;
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
bool crc32_ok(string distfile_filename, string portage_hash){
	std::string digest;
	CryptoPP::CRC32 hash;
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