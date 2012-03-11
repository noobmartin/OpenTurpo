#ifndef relayManager_h
#define relayManager_h

class relayManager{
  public:
    relayManager();
    ~relayManager();
    void update(float rpm);
    int fuelPumpOn;
    int injectorsOn;
  private:
    void enableRelays();
    void disableRelays();
    void enableFuelPump();
    void disableFuelPump();
    void enableInjectors();
    void disableInjectors();
};

#endif
