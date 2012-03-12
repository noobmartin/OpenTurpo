#include "idleManager.h"

idleManager::idleManager(){
  throttle_value = 0;
  iac_value = 0;
}

idleManager::~idleManager(){
  
}

void idleManager::update(float rpm){
  throttle_value = analogRead(throttle_sensor);
  digitalWrite(iac_motor, iac_value);
}

unsigned int idleManager::getThrottle(){
 return throttle_value; 
}

unsigned int idleManager::getIacValue(){
 return iac_value; 
}
