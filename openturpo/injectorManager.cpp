#include "injectorManager.h"

injectorManager::injectorManager(){
  air_index = 0;
  air = 0;
  duty_cycle = duty_cycle_startup;
  afr = 9.76;
  cylinders = 6;
  fuel_dens_avg = 802;
  injector_flow_rate = 350;
  inj_hrs_mass = cylinders*60*injector_flow_rate*fuel_dens_avg/1000000;
}

injectorManager::~injectorManager(){
  
}

void injectorManager::update(float rpm, ENGINE_STATE* const engine_state){
  static unsigned long last_lambda_correction = 0;
  if(rpm){
    lambda = analogRead(lambda_sensor);
    air_index = analogRead(amm_sensor);
    if(*engine_state == ENGINE_RUNNING){
      
    }
    else if(*engine_state == ENGINE_STARTING){
      // Lambda control.
      /*
      unsigned long now = micros();
      if(now - last_lambda_correction >= lambda_update_time){
        last_lambda_correction = now;
        float lambda_voltage = getLambdaVoltage();
        if(lambda_voltage > lambda_rich_threshold){
         if(duty_cycle > 0)
           duty_cycle -= 1;
        }
        else if(lambda_voltage < lambda_lean_threshold){
          if(duty_cycle < 255)
            duty_cycle += 1;
        }
      }
      */
      
      // Bosch max 370kg/h
      //air = 1.2854*pow(air_index*0.01, 3)-5.1439*pow(air_index*0.01, 2)+4.8802*air_index*0.01+8.8571;
      
      // Bosch max 480kg/h
      //air = -0.1032*pow(air_index*0.01, 5)+2.1043*pow(air_index*0.01, 4)-13.976*pow(air_index*0.01, 3)+42.148*pow(air_index*0.01, 2)-54.356*air_index*0.01+32;
      
      // Bosch max 640kg/h
      //air = -0.2171*pow(air_index*0.01, 4)+3.8659*pow(air_index*0.01, 3)-10.178*pow(air_index*0.01, 2)+5.7388*air_index*0.01+13.056;
      
      // Bosch max 850kg/h
      air = 12.186*pow(air_index*0.01, 2)-17.528*air_index*0.01+14.524;
      
      // Bosch max 1000kg/h
      //air = 10.871*pow(air_index*0.01, 2)-6.2197*air_index*0.01-2.8333;
      afr=8;
      duty_cycle = 255*(air/afr)/inj_hrs_mass;
    }
    else if(*engine_state == ENGINE_RUNNING){
      
    }
    else if(*engine_state == ENGINE_BRAKING){
      duty_cycle = 0;
    }
    else if(*engine_state == ENGINE_OFF){
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
