#include "relayManager.h"
#include "common.h"

relayManager::relayManager(){
  
}

relayManager::~relayManager(){
  
}

void relayManager::update(float rpm){
 if(!rpm){
  if(fuelPumpOn)
    disableFuelPump();
  if(injectorsOn)
    disableInjectors();
 }
 else{
  if(!fuelPumpOn)
    enableFuelPump();
  if(!injectorsOn)
    enableInjectors();
 } 
}

void relayManager::enableRelays(){
  digitalWrite(fuel_pump_relay, HIGH);
  digitalWrite(injector_relay, HIGH);
  fuelPumpOn = true;
  injectorsOn = true;
}

void relayManager::disableRelays(){
  digitalWrite(fuel_pump_relay, LOW);
  digitalWrite(injector_relay, LOW);
  fuelPumpOn = false;
  injectorsOn = false;
}

void relayManager::enableFuelPump(){
  digitalWrite(fuel_pump_relay, HIGH);
  fuelPumpOn = true;
}

void relayManager::disableFuelPump(){
  digitalWrite(fuel_pump_relay, LOW);
  fuelPumpOn = false;
}

void relayManager::enableInjectors(){
  digitalWrite(injector_relay, HIGH);
  injectorsOn = true;
}

void relayManager::disableInjectors(){
  digitalWrite(injector_relay, LOW);
  injectorsOn = false;
}
