#ifndef __STATS_H__
#define __STATS_H__

#include "connection.cpp"
#include "tui.h"
#include <sys/time.h>
using namespace std;

class Tstats{
	private:
		ulong dld_size;
		ulong dld_distfiles_count;
		uint total_size;
	public:
		ulong total_bytes_per_last_interval;
		struct timeval previous_time;
		double last_time_interval;
		uint pkg_count;
		uint distfiles_count;
		Tstats():
			dld_size(0),
			dld_distfiles_count(0),
			total_size(0),
			total_bytes_per_last_interval(0),
			previous_time(),
			last_time_interval(1),
			pkg_count(0),
			distfiles_count(0)
			{};
		void inc_dld_size(ulong more_bytes){ dld_size+=more_bytes;};
		ulong get_dld_size(){return dld_size;};
		void inc_dld_distfiles_count(){ dld_distfiles_count++;};
		ulong get_dld_distfiles_count(){return dld_distfiles_count;};
		void inc_total_size(ulong more_bytes){ total_size+=more_bytes;};
		ulong get_total_size(){return total_size;};
		void show_totals();
};

void Tstats::show_totals(){
	try{
		ulong show_last_time_interval=1;
		ulong show_total_size=1;
		if (last_time_interval>1)
			show_last_time_interval=last_time_interval;
		if (total_size>1)
			show_total_size=total_size;

		struct timeval now_timee;
		gettimeofday(&now_timee,NULL);

		msg_total("Total" 
			+field(" PKGs:",          pkg_count,4)
			+field(" = DFs:",         dld_distfiles_count,4)
			+field(" / ",             distfiles_count,4)
			+field(" = Size:",        dld_size/1000,7)
			+field(" / ",             total_size/1000,7)+" Kb "
			+field(" = ",             dld_size*100/(1+show_total_size),3)+"%%"
			+field(" Total speed: ",  (total_bytes_per_last_interval/(show_last_time_interval)),7)
			+" Kb/s"
//			+" Secs:"+toString(now_timee.tv_sec)
//			+" usecs:"+toString(now_timee.tv_usec)
			);
	}
	catch(...)
	{
		error_log_no_msg("Error in stats.cpp: show_totals()");
	}
}

Tstats stats;
#endif