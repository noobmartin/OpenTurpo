#include "common.h"
#include "ignitionManager.h"
#include "injectorManager.h"
#include "rpmManager.h"
#include <Arduino.h>

static rpmManager rpm_manager;
static injectorManager injector_manager;
static ignitionManager ignition_manager;
static logger		   logger_instance;

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

void loop(){
  rpm_manager.update();
  injector_manager.update();
  ignition_manager.setRPM(rpm_manager.getRPM());
  ignition_manager.update();
  
  static unsigned long int last_time_printed = 0;
  
  if (micros() - last_time_printed > 1000000) {
	  // RPM, dutycycle, airvoltage, lamdavoltage,
	  Serial.println("*********************************************");
	  logger_instance.logMessage("RPM", rpm_manager.getRPM());
	  logger_instance.logMessage("Duty cycle", injector_manager.getDutyCycle());
	  logger_instance.logMessage("Air voltage", injector_manager.getAirVoltage());
	  logger_instance.logMessage("Lamda voltage", injector_manager.getLambdaVoltage());
	  Serial.println("*********************************************\n");
	  last_time_printed = micros();
  }
}
