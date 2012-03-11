#include "common.h"
#include <Arduino.h>

void setup(){
  pinMode(amm_sensor_temp, INPUT);
  pinMode(amm_sensor, INPUT);
  pinMode(lambda_sensor, INPUT);
  pinMode(engine_temp_sensor, INPUT);
  pinMode(inj_bank_one, OUTPUT);
  pinMode(inj_bank_two, OUTPUT);
  pinMode(cam_sensor, INPUT);
  pinMode(crk_sensor, INPUT);
  pinMode(throttle_sensor, INPUT);
  pinMode(iac_motor, OUTPUT);
  pinMode(ign_1, OUTPUT);
  pinMode(ign_2, OUTPUT);
  pinMode(ign_3, OUTPUT);
  pinMode(ign_4, OUTPUT);
  pinMode(ign_5, OUTPUT);
  pinMode(ign_6, OUTPUT);
  pinMode(fuel_pump_relay, OUTPUT);
  pinMode(injector_relay, OUTPUT);
  pinMode(fan_lo_relay, OUTPUT);
  pinMode(fan_hi_relay, OUTPUT);
  
  Serial.begin(serial_baud_rate);
}

logger logger_instance;

void loop(){
	logger_instance.logMessage("RPM", 2500);
	logger_instance.logMessage("AMM", 250.0f);
}
