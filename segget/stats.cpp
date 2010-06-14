#include "tui.cpp"
class Tstats{
  private:
    ulong downloaded_size;
    uint total_size;
  public:
    uint pkg_count;
    uint distfiles_count;
    void inc_downloaded_size(ulong more_bytes){ downloaded_size+=more_bytes;};
    ulong get_downloded_size(){return downloaded_size;};
    void inc_total_size(ulong more_bytes){ total_size+=more_bytes;};
    ulong get_total_size(){return total_size;};
    void show_totals();
};

void Tstats::show_totals(){
    msg_total( 
      "Total: PKGs:"   + toString(pkg_count)
      +" = Distfiles:" + toString(distfiles_count)
      +" = Bytes:"+ toString(downloaded_size/1024)+"/"+toString(total_size/1024)+"Kb"
      +" = "+toString(downloaded_size*100/total_size)+" percent");
}

Tstats stats;

