#include <string>

using namespace std;

class Tsettings{
  private:
    bool resume_on;
  public:
    string distfile_dir;
    Tsettings(): resume_on(1),distfile_dir(""){};
    void set_resume(bool resume_setting){resume_on=resume_setting;};
    bool get_resume(){return resume_on;};
};

Tsettings settings;