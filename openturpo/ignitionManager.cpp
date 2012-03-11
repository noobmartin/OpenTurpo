#include "ignitionManager.h"

ignitionManager::ignitionManager(){
  spark_charge_time_us = 4000;
  zero_start_time = 0;
  ign_state = IGN_FIND_TRIGGER;
}

ignitionManager::~ignitionManager(){
  
}

void ignitionManager::update(){
  
}
