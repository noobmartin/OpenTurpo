#ifndef idleManager_h
#define idleManager_h

#include "common.h"

class idleManager{
  public:
    idleManager();
    ~idleManager();
    void update(float rpm);
    unsigned int getThrottle();
  private:
    unsigned int throttle_value;
};

#endif
