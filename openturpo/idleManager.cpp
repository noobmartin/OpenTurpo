#include "idleManager.h"

idleManager::idleManager(){
  throttle_value = 0;
}

idleManager::~idleManager(){
  
}

void idleManager::update(float rpm){
  throttle_value = analogRead(throttle_sensor);
  digitalWrite(iac_motor, 0);
}

unsigned int idleManager::getThrottle(){
 return throttle_value; 
}
