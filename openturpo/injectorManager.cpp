#include "injectorManager.h"

injectorManager::injectorManager(){
  air_index = 0;
  air = 0;
  duty_cycle = 0;
  afr = 9.76;
  cylinders = 6;
  fuel_dens_avg = 0.802;
  injector_flow_rate = 350;
  inj_hrs_mass = injector_flow_rate*60*fuel_dens_avg/1000;
}

injectorManager::~injectorManager(){
  
}

void injectorManager::update(float rpm, ENGINE_STATE* const engine_state){
  if(rpm){
    air_index = analogRead(amm_sensor);
    air = 10.07*pow(air_index, 1.9544345015);
    duty_cycle = 255*(air/(afr*cylinders*inj_hrs_mass));
    lambda = analogRead(lambda_sensor);
  }
  else if(rpm == 0)
    duty_cycle = 0;
  analogWrite(inj_bank_one, duty_cycle);
  analogWrite(inj_bank_two, duty_cycle);
}

float injectorManager::getAir(){
 return air; 
}

float injectorManager::getAirVoltage(){
 return (float)(5/1024)*air_index; 
}

byte injectorManager::getDutyCycle(){
  return duty_cycle;
}

unsigned int injectorManager::getLambda() {
  return lambda;
}

float injectorManager::getLambdaVoltage() {
  return (5 * lambda) / 1024;
}
