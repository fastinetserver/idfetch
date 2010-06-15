#include "connection.cpp"
#include "tui.cpp"

class Tstats{
  private:
    ulong downloaded_size;
    uint total_size;
  public:
    ulong total_bytes_per_last_interval;
    ulong last_time_interval;
    uint pkg_count;
    uint distfiles_count;
    void inc_downloaded_size(ulong more_bytes){ downloaded_size+=more_bytes;};
    ulong get_downloded_size(){return downloaded_size;};
    void inc_total_size(ulong more_bytes){ total_size+=more_bytes;};
    ulong get_total_size(){return total_size;};
    void show_totals();
};

void Tstats::show_totals(){
    msg_total("Total" 
      +field(" PKGs:",pkg_count,4)
      +field(" = Distfiles:",distfiles_count,4)
      +field(" = Size:",downloaded_size/1024,7)
      +field(" / ",total_size/1024,7)+" Kb "
      +field(" = ",downloaded_size*100/total_size,3)+"%%"
      +field(" Total speed: ",(total_bytes_per_last_interval*1000/(1+last_time_interval)),7)+" Kb/s");
}

Tstats stats;

