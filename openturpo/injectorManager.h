#ifndef injectorManager_h
#define injectorManager_h

#include <Arduino.h>

class injectorManager{
  public:
     injectorManager();
     ~injectorManager();
     void update(float rpm);
     float getAirVoltage();
     float getAir();
     byte getDutyCycle();
     unsigned int getLambda();
     float getLambdaVoltage();
  private:
    int injector_flow_rate;
    int cylinders;
    unsigned int air_index;
    float air;
    byte duty_cycle;
    float afr;
    float inj_hrs_mass;
    float fuel_dens_avg;
    unsigned int lambda;
};

#endif
