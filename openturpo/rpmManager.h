#ifndef rpmManager_h
#define rpmManager_h

#include "common.h"

class rpmManager{
  public:
    rpmManager();
    ~rpmManager();
    void update(ENGINE_STATE* engine_state);
    float getRPM();
  private:
    unsigned long zero_start_time;
    unsigned int nonzero_captured;
    float rpm;
};

#endif
