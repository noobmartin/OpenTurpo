#ifndef idleManager_h
#define idleManager_h

#include "common.h"

class idleManager{
  public:
    idleManager();
    ~idleManager();
    void update(float rpm);
    unsigned int getThrottle();
    unsigned int getIacValue();
  private:
    unsigned int iac_value;
    unsigned int throttle_value;
};

#endif
