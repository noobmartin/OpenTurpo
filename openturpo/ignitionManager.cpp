#include "ignitionManager.h"

ignitionManager::ignitionManager(){
  spark_charge_time_us = 4000;
  ign_state = IGN_FIND_TRIGGER;
  rpm = 0;
  ignition_advance = 15;
}

ignitionManager::~ignitionManager(){
  
}

void ignitionManager::update(){
  // Linear advance curve from 10 to 40 degrees.
  ignition_advance = (30/7000)*rpm+10;
  
  switch (ign_state){
   case IGN_FIND_TRIGGER:
    findTrigger();
    break;
   case IGN_WAIT_FIRST_PAIR:
    waitFirstPair();
    break;
   case IGN_WAIT_SECOND_PAIR:
    waitSecondPair();
    break;
   case IGN_WAIT_THIRD_PAIR:
    waitThirdPair();
    break; 
  }
}

void ignitionManager::findTrigger(){
  unsigned int crk_value = analogRead(crk_sensor);
  static unsigned long first_zero_time = 0;
  if(crk_value >= positive_crank_threshold){
    first_zero_time = 0;
    return;
  }
  else if(crk_value == 0){
   if(first_zero_time == 0){
    first_zero_time = micros();
    return;
   }
   else{
     unsigned long time = micros();
     unsigned long consecutive_zero_time = time - first_zero_time;
     unsigned long trigger_time = 1.5*(60000000/rpm)/(crank_teeth+2);
     if(consecutive_zero_time >= trigger_time){
       ign_state = IGN_WAIT_FIRST_PAIR;
       first_zero_time = 0;
       return; 
     }
     else
       return;
   }
  }
}

void ignitionManager::waitFirstPair(){
   float usec_per_deg = 60000000/(rpm*360);
   float wait_deg_before_fire = cmp_interrupt_deg_before_tdc - ignition_advance;
   float wait_usec_before_fire = usec_per_deg*wait_deg_before_fire - spark_charge_time_us;
   
  static unsigned long enter_time = 0;
  unsigned long time = micros();
  unsigned long wait_time = time - enter_time;
  if(enter_time == 0){
    enter_time = time;
    return;
  }
  else if(wait_time >= wait_usec_before_fire){
    fireFirstPair();
    ign_state = IGN_WAIT_SECOND_PAIR;
    enter_time = 0;
    return; 
  }
  else
    return;
}

void ignitionManager::waitSecondPair(){
  float usec_per_deg = 60000000/(rpm*360);
  float wait_deg_before_fire = cmp_interrupt_deg_before_tdc - ignition_advance;
  float wait_usec_before_fire = usec_per_deg*cylinder_deg_offset - spark_charge_time_us;
   
  static unsigned long enter_time = 0;
  unsigned long time = micros();
  unsigned long wait_time = time - enter_time;
  if(enter_time == 0){
    enter_time = time;
    return;
  }
  else if(wait_time >= wait_usec_before_fire){
    fireSecondPair();
    ign_state = IGN_WAIT_THIRD_PAIR;
    enter_time = 0;
    return; 
  }
  else
    return;
}

void ignitionManager::waitThirdPair(){
  float usec_per_deg = 60000000/(rpm*360);
  float wait_deg_before_fire = cmp_interrupt_deg_before_tdc - ignition_advance;
  float wait_usec_before_fire = usec_per_deg*cylinder_deg_offset - spark_charge_time_us;
   
  static unsigned long enter_time = 0;
  unsigned long time = micros();
  unsigned long wait_time = time - enter_time;
  if(enter_time == 0){
    enter_time = time;
    return;
  }
  else if(wait_time >= wait_usec_before_fire){
    fireThirdPair();
    enter_time = 0;
    ign_state = IGN_FIND_TRIGGER;
    return; 
  }
  else
    return;
}

void ignitionManager::fireFirstPair(){
   digitalWrite(ign_1, HIGH);
   digitalWrite(ign_6, HIGH);
   delayMicroseconds(spark_charge_time_us);
   digitalWrite(ign_1, LOW);
   digitalWrite(ign_6, LOW);
}

void ignitionManager::fireSecondPair(){
   digitalWrite(ign_5, HIGH);
   digitalWrite(ign_2, HIGH);
   delayMicroseconds(spark_charge_time_us);
   digitalWrite(ign_5, LOW);
   digitalWrite(ign_2, LOW);
}

void ignitionManager::fireThirdPair(){
   digitalWrite(ign_3, HIGH);
   digitalWrite(ign_4, HIGH);
   delayMicroseconds(spark_charge_time_us);
   digitalWrite(ign_3, LOW);
   digitalWrite(ign_4, LOW);
}

void ignitionManager::setRPM(float srpm){
  rpm = srpm; 
}
