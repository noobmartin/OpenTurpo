#ifndef ignitionManager_h
#define ignitionManager_h

class ignitionManager{
  public:
    ignitionManager();
    ~ignitionManager();
    void update();
  private:
    unsigned long zero_start_time;
    int spark_charge_time_us;
};

#endif
