#ifndef ignitionManager_h
#define ignitionManager_h

#include "common.h"

class ignitionManager{
  public:
    ignitionManager();
    ~ignitionManager();
    void update(ENGINE_STATE* const engine_state);
    void findTrigger();
    void waitFirstPair();
    void waitSecondPair();
    void waitThirdPair();
    void fireFirstPair();
    void fireSecondPair();
    void fireThirdPair();
    void setRPM(float srpm);
    float getIgnitionAdvance();
  private:
    float ignition_advance;
    float rpm;
    int spark_charge_time_us;
    IGNITION_STATE ign_state;
};

#endif
