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
  static unsigned long last_lambda_correction = 0;
  if(rpm){
    lambda = analogRead(lambda_sensor);
    air_index = analogRead(amm_sensor);
    if(*engine_state == ENGINE_RUNNING){
      air = 10.07*pow(air_index, 1.9544345015);
      duty_cycle = 255*(air/(afr*cylinders*inj_hrs_mass));
    }
    else if(*engine_state == ENGINE_STARTING){
      unsigned long now = micros();
      if(now - last_lambda_correction >= lambda_update_time){
        last_lambda_correction = now;
        float lambda_voltage = getLambdaVoltage();
        if(lambda_voltage > lambda_rich_threshold){
         duty_cycle -= 1; 
        }
        else if(lambda_voltage < lambda_lean_threshold){
          duty_cycle += 1;
        }
      }
    }
    else if(*engine_state == ENGINE_RUNNING){
      
    }
    else if(*engine_state == ENGINE_BRAKING){
      duty_cycle = 0;
    }
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
 return (5.0f/1024.0f)*air_index; 
}

byte injectorManager::getDutyCycle(){
  return duty_cycle;
}

unsigned int injectorManager::getLambda() {
  return lambda;
}

float injectorManager::getLambdaVoltage() {
  return (5.0f * lambda) / 1024.0f;
}
