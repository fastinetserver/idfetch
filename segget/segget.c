#include <sstream> 
#include <fstream>
#include <cstring>
#include <string>
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <curl/curl.h>
	int distfile_size=110435;
	int segment_size=20000;

using namespace std;

template<typename T> std::string toString(T t) 
{ 
    std::stringstream s; 
    s << t; 
    return s.str(); 
} 
           
unsigned long downloaded_bytes=0;
size_t write_data(void *buffer, size_t size, size_t nmemb, void *cur_segment);


class Tdistfile{
	private:
		char* urllist[];
	public:
		void* nextsegment(){};
};

class Tsegment{
	private:
		string file_name;
		char* urllist[];
	public:
		unsigned long downloaded_bytes;
		string url;
		string range;
		FILE *segment_file;
		Tsegment(string segment_url, string segment_file_name, string segment_range);
		~Tsegment();
		string get_file_name(){return file_name;};
		int fetch();
		
};

Tsegment::Tsegment(string segment_url, string segment_file_name, string segment_range){
	url=segment_url;
	range=segment_range;
	downloaded_bytes=0;
	file_name=segment_file_name;
	//try
		segment_file = fopen(file_name.c_str(), "w" );
}
Tsegment::~Tsegment(){
	//try
		fclose(segment_file);
}



int Tsegment::fetch(){
  CURL *easyhandle;
  CURLcode result;

  easyhandle = curl_easy_init();
  cout << "Started downloading\n";

  if(easyhandle) {
    curl_easy_setopt(easyhandle, CURLOPT_URL, url.c_str() );
//    curl_easy_setopt(easyhandle, CURLOPT_URL, "http://www.mail.ru");
    curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(easyhandle, CURLOPT_RANGE, range.c_str());
    curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, write_data);
    result = curl_easy_perform(easyhandle);

    cout << "RESULT:" << result << "\n";
     // always cleanup
    curl_easy_cleanup(easyhandle);
    return result;
  }
}

int main(void)
{	
	string url_list[]={
                    "ftp://gentoo.mirrors.tds.net/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "ftp://ftp.lug.udel.edu/pub/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://www.gtlib.gatech.edu/pub/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "ftp://ftp.wallawalla.edu/pub/mirrors/ftp.gentoo.org/distfiles/randrproto-1.3.1.tar.bz2", 
                    "ftp://gentoo.netnitco.net/pub/mirrors/gentoo/source/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://gentoo.mirrors.easynews.com/linux/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://gentoo.cites.uiuc.edu/pub/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "ftp://distro.ibiblio.org/pub/linux/distributions/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://mirror.usu.edu/mirrors/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "ftp://gentoo.chem.wisc.edu/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://chi-10g-1-mirror.fastsoft.net/pub/linux/gentoo/gentoo-distfiles/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://gentoo.osuosl.org/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://gentoo.mirrors.hoobly.com/distfiles/randrproto-1.3.1.tar.bz2", 
                    "ftp://mirror.its.uidaho.edu/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "ftp://gentoo.llarian.net/pub/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "ftp://mirrors.rit.edu/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://www.cyberuse.com/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://mirror.datapipe.net/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://gentoo.mirrors.tds.net/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://mirrors.cs.wmich.edu/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://distro.ibiblio.org/pub/linux/distributions/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://mirrors.rit.edu/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "ftp://mirror.iawnet.sandia.gov/pub/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "ftp://ftp.ussg.iu.edu/pub/linux/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://mirror.mcs.anl.gov/pub/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://gentoo.cs.uni.edu/distfiles/randrproto-1.3.1.tar.bz2", 
                    "ftp://gentoo.cites.uiuc.edu/pub/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "ftp://mirror.datapipe.net/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://ftp.ucsb.edu/pub/mirrors/linux/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://lug.mtu.edu/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://mirror.its.uidaho.edu/pub/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "ftp://lug.mtu.edu/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://gentoo.chem.wisc.edu/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://gentoo.llarian.net/distfiles/randrproto-1.3.1.tar.bz2", 
                    "ftp://ftp.ucsb.edu/pub/mirrors/linux/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://gentoo.netnitco.net/distfiles/randrproto-1.3.1.tar.bz2", 
                    "ftp://mirror.mcs.anl.gov/pub/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://mirror.lug.udel.edu/pub/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "ftp://chi-10g-1-mirror.fastsoft.net/pub/linux/gentoo/gentoo-distfiles/distfiles/randrproto-1.3.1.tar.bz2", 
                    "ftp://ftp.gtlib.gatech.edu/pub/gentoo/distfiles/randrproto-1.3.1.tar.bz2", 
                    "http://xorg.freedesktop.org/releases/individual/proto/randrproto-1.3.1.tar.bz2"

	};
	int url_num=0;
	int segments_count=distfile_size/segment_size;
	cout << "Need to download: " << segments_count+1 << " segments\n";
	for (int segment_num=0; segment_num <= segments_count; segment_num++){
		int fetcher_result=1000;
		do {
			cout << "=============================\n";
			int range_end;
			if (segment_num == segments_count)
				range_end=distfile_size;
			else
				range_end=(segment_num+1)*segment_size-1;
			Tsegment fetcher(url_list[url_num],"distfile.seg"+toString(segment_num), toString(segment_num*segment_size)+"-"+toString(range_end));
			cout << fetcher.get_file_name() << "\n";
			url_num++;
			if (url_num >= sizeof(url_list)/sizeof(string))
				url_num=0;
			fetcher_result=fetcher.fetch();
		} while (fetcher_result);
	}

	cout << "Combining files \n";
	FILE* segment_file;
	FILE* complete_file;
	complete_file = fopen("dist.tar.bz2","w");
	char buffer[segment_size];
	for (int segment_num=0; segment_num <= segments_count; segment_num++){
		cout << "joining segment " << segment_num << "\n";
		segment_file = fopen(("distfile.seg"+toString(segment_num)).c_str(),"r");
		int read_bytes=fread(buffer, 1, segment_size, segment_file);
		fwrite(buffer,read_bytes, 1, complete_file);
		fclose(segment_file);
	}
	fclose(complete_file);
	cout << segments_count << " files combined\n";
	return 0;
}

size_t write_data(void *buffer, size_t size, size_t nmemb, void *cur_segment){
    Tsegment *segment;
    segment =(Tsegment*)cur_segment;
    segment->downloaded_bytes+=nmemb;
    fwrite(buffer,size,nmemb,segment->segment_file);
    cout << "DOWNLOADING:" << segment->get_file_name()<< " range:"<<segment->range<<" " << segment->downloaded_bytes << " b = " << (segment->downloaded_bytes*100/segment_size) << "%\n";
    return nmemb;
}