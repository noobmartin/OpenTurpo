#include "idleManager.h"

idleManager::idleManager(){
  
}

idleManager::~idleManager(){
  
}

void idleManager::update(float rpm){
  digitalWrite(iac_motor, 0);
}
