#ifndef __STATS_H__
#define __STATS_H__

#include "connection.cpp"
#include "checksum.cpp"

class Tstats{
  private:
    ulong dld_size;
    ulong dld_distfiles_count;
    uint total_size;
  public:
    ulong total_bytes_per_last_interval;
    ulong last_time_interval;
    uint pkg_count;
    uint distfiles_count;
    void inc_dld_size(ulong more_bytes){ dld_size+=more_bytes;};
    ulong get_dld_size(){return dld_size;};
    void inc_dld_distfiles_count(){ dld_distfiles_count++;};
    ulong get_dld_distfiles_count(){return dld_distfiles_count;};
    void inc_total_size(ulong more_bytes){ total_size+=more_bytes;};
    ulong get_total_size(){return total_size;};
    void show_totals();
};

void Tstats::show_totals(){
    msg_total("Total" 
      +field(" PKGs:",          pkg_count,4)
      +field(" = DFs:",         dld_distfiles_count,4)
      +field(" / ",             distfiles_count,4)
      +field(" = Size:",        dld_size/1000,7)
      +field(" / ",             total_size/1000,7)+" Kb "
      +field(" = ",             dld_size*100/total_size,3)+"%%"
      +field(" Total speed: ",  (total_bytes_per_last_interval/1000/(1+last_time_interval)),7)+" Kb/s");
}

Tstats stats;
#endif