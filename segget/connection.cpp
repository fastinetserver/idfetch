#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <time.h>

class Tconnection{
  private:
    ulong bytes_per_last_interval;
  public:
    void *segment;
	Tconnection():bytes_per_last_interval(0),segment(0){};
    void  inc_bytes_per_last_interval(ulong new_bytes_count){bytes_per_last_interval+=new_bytes_count;};
    ulong get_bytes_per_last_interval(){return bytes_per_last_interval;};
    void  reset_bytes_per_last_interval(){bytes_per_last_interval=0;};
};

#define MAX_CONNECTS 6 /* number of simultaneous transfers */
time_t prev_time;
Tconnection connection_array[MAX_CONNECTS];
#endif