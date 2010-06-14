class Tsettings{
  private:
    bool resume_on;
  public:
    void set_resume(bool resume_setting){resume_on=resume_setting;};
    bool get_resume(){return resume_on;};
};

Tsettings settings;