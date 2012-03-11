#include "rpmManager.h"
#include "common.h"

rpmManager::rpmManager(){
  rpm = 0;
  zero_start_time = 0;
  nonzero_captured = 0;
}

rpmManager::~rpmManager(){
  
}

void rpmManager::update(){
  int crk_value = analogRead(crk_sensor);
  
  if(crk_value >= positive_crank_threshold){
   if(!nonzero_captured){
    nonzero_captured = true;
    return; 
   }
  }
  else if(crk_value == 0){
   if(nonzero_captured){
    unsigned long now = micros();
    unsigned long pulse_time = now - zero_start_time;
    
    rpm = 60000000/(pulse_time*crank_teeth);
    
    nonzero_captured = 0;
    zero_start_time = now;
   }
   else{
    zero_start_time = micros();
   } 
  }
}

float rpmManager::getRPM(){
 return rpm; 
}
