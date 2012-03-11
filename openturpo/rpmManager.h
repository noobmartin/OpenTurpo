#ifndef rpmManager_h
#define rpmManager_h

class rpmManager{
  public:
    rpmManager();
    ~rpmManager();
    void update();
    float getRPM();
  private:
    unsigned long zero_start_time;
    unsigned int nonzero_captured;
    float rpm;
};

#endif
