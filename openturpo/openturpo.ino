#include "common.h"
#include "ignitionManager.h"
#include "injectorManager.h"
#include "rpmManager.h"
#include "idleManager.h"
#include "relayManager.h"
#include <Arduino.h>

static rpmManager rpm_manager;
static injectorManager injector_manager;
static ignitionManager ignition_manager;
static idleManager idle_manager;
static relayManager relay_manager;
static logger logger_instance;
ENGINE_STATE engine_state;

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
  
  /**********************************************************************************/
  // Set pwm resolution  to mode 7 (10 bit) (Credits to jabber @ arduino.cc forums
  // Timer1A is connected to pins 11 & 12
  /**********************************************************************************/
//  TCCR1B &= ~(1 << WGM13);    // Timer B clear bit 4
//  TCCR1B |=  (1 << WGM12);    // set bit 3
//  TCCR1A |= (1 << WGM11);    //  Timer A set bit 1
//  TCCR1A |= (1 << WGM10);    //  set bit 0
  
  engine_state = ENGINE_OFF;
}

void loop(){
  rpm_manager.update(&engine_state);
  float rpm = rpm_manager.getRPM();
  relay_manager.update(rpm);
  injector_manager.update(rpm, &engine_state);
  ignition_manager.setRPM(rpm);
  ignition_manager.update(&engine_state);
  idle_manager.update(rpm, &engine_state);
  
  static unsigned long int last_time_printed = 0;
  
  if (micros() - last_time_printed > 1000000) {
	  // RPM, dutycycle, airvoltage, lamdavoltage,
	  Serial.println("*********************************************");

          if(engine_state == ENGINE_OFF)
            logger_instance.logMessage("Engine off");
          else if(engine_state == ENGINE_STARTING)
            logger_instance.logMessage("Engine starting");
          else if(engine_state == ENGINE_IDLING)
            logger_instance.logMessage("Engine idling");
          else if(engine_state == ENGINE_RUNNING)
            logger_instance.logMessage("Engine running");
          else if(engine_state == ENGINE_BRAKING)
            logger_instance.logMessage("Engine braking");
          else
            logger_instance.logMessage("Engine state unrecognized");
            
          if(relay_manager.getFuelPumpState())
            logger_instance.logMessage("Fuel pump: ON");
          else
            logger_instance.logMessage("Fuel pump: OFF");
            
          if(relay_manager.getInjectorsState())
            logger_instance.logMessage("Injectors: ON");
          else
            logger_instance.logMessage("Injectors: OFF");
            
          logger_instance.logMessage("Idle motor value", idle_manager.getIacValue());
	  logger_instance.logMessage("RPM", rpm_manager.getRPM());
          logger_instance.logMessage("Ignition advance", ignition_manager.getIgnitionAdvance());
	  logger_instance.logMessage("Duty cycle", injector_manager.getDutyCycle());
	  logger_instance.logMessage("Air voltage", injector_manager.getAirVoltage());
          logger_instance.logMessage("Air (kg/h)", injector_manager.getAir());
	  logger_instance.logMessage("Lamda voltage", injector_manager.getLambdaVoltage());
	  Serial.println("*********************************************\n");
	  last_time_printed = micros();
  }
  
}
