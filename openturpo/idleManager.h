#ifndef idleManager_h
#define idleManager_h

#include "common.h"

class idleManager{
  public:
    idleManager();
    ~idleManager();
    void update(float rpm, ENGINE_STATE* engine_state);
    unsigned int getThrottle();
    unsigned int getIacValue();
  private:
    unsigned int iac_value;
    unsigned int throttle_value;
};

#endif
