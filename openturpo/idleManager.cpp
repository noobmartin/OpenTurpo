#include "idleManager.h"

idleManager::idleManager(){
  
}

idleManager::~idleManager(){
  
}

void idleManager::update(){
  digitalWrite(iac_motor, 0);
}
