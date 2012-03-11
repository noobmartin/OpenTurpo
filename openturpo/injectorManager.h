#ifndef injectorManager_h
#define injectorManager_h

class injectorManager{
  public:
     injectorManager();
     ~injectorManager();
     void update(float rpm);
     float getAirVoltage();
     float getAir();
     unsigned int getDutyCycle();
     unsigned int getLambda();
     float getLambdaVoltage();
  private:
    int injector_flow_rate;
    int cylinders;
    unsigned int air_index;
    float air;
    unsigned int duty_cycle;
    float afr;
    float inj_hrs_mass;
    float fuel_dens_avg;
    unsigned int lambda;
};

#endif
