#ifndef common_h
#define common_h

#include <Arduino.h>
#include "logger.h"

#define positive_crank_threshold 5
#define crank_teeth 58
#define inj_bank_one 2
#define inj_bank_two 4
#define crk_sensor A8
#define amm_sensor A15
#define amm_sensor_temp  A1
#define lambda_sensor  A3
#define engine_temp_sensor A5
#define knock_sensor_front A7
#define knock_sensor_rear A9
#define throttle_sensor A11
#define cam_sensor A13
#define iac_motor 10
#define ign_1 33
#define ign_2 37
#define ign_3 41
#define ign_4 45
#define ign_5 49
#define ign_6 53
#define fuel_pump_relay 25
#define injector_relay 30
#define fan_lo_relay 22
#define fan_hi_relay 29
#define cylinder_deg_offset 120
#define cmp_interrupt_deg_before_tdc 90
#define serial_baud_rate 115200

enum ENGINE_STATE{
  ENGINE_OFF,
  ENGINE_STARTING,
  ENGINE_RUNNING,
  ENGINE_IDLING
};

enum IGNITION_STATE{
  IGN_FIND_TRIGGER,
  IGN_WAIT_FIRST_PAIR,
  IGN_WAIT_SECOND_PAIR,
  IGN_WAIT_THIRD_PAIR
};

#endif
