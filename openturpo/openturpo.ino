
/* The following are command definitions for the
 * serial protocol used to control engine parameters
 * and monitor sensors in real-time.
 */
enum{
  SET  = 'a',
  READ = 'b'
}COMMAND;

enum{
  ALL =             'a',
  LAMBDA =          'b',
  FUEL_PUMP_RELAY = 'c',
  AMM =             'd',
  AMM_TEMP =        'e',
  ENGINE_TEMP =     'f',
  INJECTOR_RELAY =  'g',
  DME_RELAY =       'h',
  RPM =             'i',
  FUEL_CONSUMPTION = 'j',
  FAN =             'k',
  IAC =             'l'
}DEVICE;

enum{
  ON =   'a',
  OFF =  'b'
}OPERATION;

enum{
  DISABLED =  'a',
  LO =        'b',
  HI =        'c'
}FAN_MODE;

typedef struct{
  byte iac;
  byte fan;
  byte dme_relay;
  byte fuel_pump;
  byte inj_relay;
  float lambda;
  float amm;
  float amm_temp;
  float rpm;
  float engine_temp;
  float fuel_consumption;
}full_sensor_data;

/* This value is for E85.
 * For unleaded gasoline, use 14.7
 */
static float afr = 9.76;

/* The following is a table of the density of pure
 * ethanol at various temperatures in centigrade and g/cc.
 */
float fuel_dens_avg = 0.802;
float fuel_density_temperatures[32] =   
{3,4,5,6,7,8,9,10,11,12,16,17,18,19,20,21,22,23,24,25,29,30,31,32,33,34,35,36,37,38,39,40};
float fuel_density[32] =
{
    0.8037,0.80290,0.80207,0.80123,0.80039,0.79956,0.79872,
    0.79788,0.79704,0.79620,0.79283,0.79198,0.79114,0.79029,
    0.78945,0.78860,0.78775,0.78691,0.78606,0.78522,0.78182,
    0.78097,0.78012,0.77927,0.77841,0.77756,0.77671,0.77585,
    0.77500,0.77414,0.77329,0.77244
};

/* This value represents the injector flow rates.
 * It is in the units of cc/min @ 3 bar of pressure.
 */
float inj = 350;
float inj_hrs = inj*60;
float inj_hrs_mass = inj_hrs*fuel_dens_avg/1000;

int cylinders = 6;
int flywheel_teeth = 142;
int cylinder_deg_offset = 120;
int cmp_interrupt_deg_before_tdc = 0;
int spark_charge_usec = 4000;  // 4000us coil charge time (4ms).

int amm = A0;
int amm_temp = A2;
int lambda = A4;
int engine_temp = A6;
int knock_front = A8
int knock_rear = A10
int tps = A12;
int cmp = 2;
int crk = 4;
int injector_bank_one = 6;
int injector_bank_two = 8;
int iac = 10;
int tach = 12;
int ign_1 = 22;
int ign_2 = 26;
int ign_3 = 30;
int ign_4 = 23;
int ign_5 = 49;
int ign_6 = 53;
int fuel_pump = 39;
int inj_relay = 36;
int dme_relay = 31;
int fan_lo = 35;
int fan_hi = 43;

void setup(){
  /* Set the correct input/output pins. */
  pinMode(amm_temp, INPUT);
  pinMode(amm, INPUT);
  pinMode(lambda, INPUT);
  pinMode(engine_temp, INPUT);
  pinMode(injector_bank_one, OUTPUT);
  pinMode(injector_bank_two, OUTPUT);
  pinMode(cmp, INPUT);
  pinMode(crk, INPUT);
  pinMode(iac, OUTPUT);
  pinMode(ign_1, OUTPUT);
  pinMode(ign_2, OUTPUT);
  pinMode(ign_3, OUTPUT);
  pinMode(ign_4, OUTPUT);
  pinMode(ign_5, OUTPUT);
  pinMode(ign_6, OUTPUT);
  pinMode(fuel_pump, OUTPUT);
  pinMode(inj_relay, OUTPUT);
  pinMode(dme_relay, OUTPUT);
  pinMode(fan_lo, OUTPUT);
  pinMode(fan_hi, OUTPUT);
  
  analogWrite(iac, 255);
  
  /* Set up interrupts. */
  attachInterrupt(crk, crank_trigger, RISING);
  attachInterrupt(cmp, ignition, RISING);
  
  /* Set up serial communication. */
  Serial.begin(115200);
}

void loop(){
  /* In here we continuously adjust fuel injector
   * duty cycle based on air entering the engine.
   * The operation is simple, and uses the signal from
   * the air mass meter to set the correct fuel amount
   * on average based on the air:fuel ratio.
   */
  float air = analogRead(amm);
  byte duty_cycle = 255*(air/(afr*cylinders))/inj_hrs_mass;
  analogWrite(injector_bank_one, duty_cycle);
  analogWrite(injector_bank_two, duty_cycle);
}

void ignition(){
  /* This function is interrput triggered.
   * It is used for precise ignition timing for
   * all cylinders.
   * All cylinders complete their total four strokes
   * in two engine revolutions, i.e 720 degrees.
   * Firing order is 1-5-3-6-2-4.
   */
   detachInterrupt(cmp);
   
   // This can be adjusted to vary ignition timing in degrees depending on engine load, RPM etc.
   int fire_deg_before_tdc = 15;

   // As engine RPM varies, so does the time corresponding to one degree.
   float usec_per_deg = ((60/(RPM))/360)*1000000;
   
   // The interrupt is triggered cmp_interrupt_deg_before_tdc degrees before tdc.
   int sleep_deg_before_fire = cmp_interrupt_deg_before_tdc - fire_deg_before_tdc;

   // We also need to take in to account the time it takes to charge the coil.
   float sleep_usec_before_fire = usec_per_deg*sleep_deg_before_fire - spark_charge_usec;
   
   // First cylinder - the coil is fired when the signal goes low.
   delayMicroseconds(sleep_usec_before_fire);
   digitalWrite(ign_1, HIGH);
   delayMicroseconds(spark_charge_usec);
   digitalWrite(ign_1, LOW);

   // After the first cylinder, all remaining cylinders have the same time offsets.
   sleep_usec_before_fire = usec_per_deg*cylinder_deg_offset - spark_charge_usec;
   
   // Fifth cylinder.
   delayMicroseconds(sleep_usec_before_fire);
   digitalWrite(ign_5, HIGH);
   delayMicroseconds(spark_charge_usec);
   digitalWrite(ign_5, LOW);
   
   // Third cylinder.
   delayMicroseconds(sleep_usec_before_fire);
   digitalWrite(ign_3, HIGH);
   delayMicroseconds(spark_charge_usec);
   digitalWrite(ign_3, LOW);
   
   // Sixth cylinder.
   delayMicroseconds(sleep_usec_before_fire);
   digitalWrite(ign_6, HIGH);
   delayMicroseconds(spark_charge_usec);
   digitalWrite(ign_6, LOW);
   
   // Second cylinder.
   delayMicroseconds(sleep_usec_before_fire);
   digitalWrite(ign_2, HIGH);
   delayMicroseconds(spark_charge_usec);
   digitalWrite(ign_2, LOW);
   
   // Fourth cylinder.
   delayMicroseconds(sleep_usec_before_fire);
   digitalWrite(ign_4, HIGH);
   delayMicroseconds(spark_charge_usec);
   digitalWrite(ign_4, LOW);

   attachInterrupt(cmp, ignition, RISING);
}

void crank_trigger(){
  /* In this function we use the trigger pulses from
   * the crankshaft to continuously update engine
   * RPM.
   */
  static unsigned long last_time = 0;
  static int pulses = 0;
  if(pulses >= flywheel_teeth){
    unsigned long current_time = micros();
    unsigned long time_diff = current_time - last_time;
    static float RPM = 60000000/time_diff;
    pulses = 0;
  }
  else
    pulses++;
}

void serialEvent(){
  /* This function handles communication with the outside world
   * via the serial communication interface.
   */
   byte command = Serial.read();
   Serial.print("Command received");
   Serial.flush();
   switch(command){
    case SET:
      processSet();
      break;
    case READ:
      processRead();
      break;
   }
}

void processSet(){
   byte device = Serial.read();
   Serial.flush();
   byte operation = Serial.read();
   Serial.flush();
   
   switch(device){
    case ALL:
      if(operation == ON){
        digitalWrite(fuel_pump, HIGH);
        digitalWrite(inj_relay, HIGH);
        digitalWrite(dme_relay, HIGH);
        Serial.println("All relays online.");
      }
      else{
        digitalWrite(fuel_pump, LOW);
        digitalWrite(inj_relay, LOW);
        digitalWrite(dme_relay, LOW);
        Serial.println("All relays offline.");
      }
      break;
     
    case FUEL_PUMP_RELAY:
      if(operation == ON){
        digitalWrite(fuel_pump, HIGH);
        Serial.println("Fuel pump online.");
      }
      else{
        digitalWrite(fuel_pump, LOW);
        Serial.println("Fuel pump offline."); 
      }
      break;
    case INJECTOR_RELAY:
     if(operation == ON){
       digitalWrite(inj_relay, HIGH);
       Serial.println("Injectors online."); 
     }
     else{
       digitalWrite(inj_relay, LOW);
       Serial.println("Injectors offline."); 
     }
     break;
    case DME_RELAY:
     if(operation == ON){
       digitalWrite(dme_relay, HIGH);
       Serial.println("DME relay online."); 
     }
     else{
       digitalWrite(dme_relay, LOW);
       Serial.println("DME relay offline.");
     }
     break;
     case FAN:
      if(operation == DISABLED){
        digitalWrite(fan_hi, LOW);
        digitalWrite(fan_lo, LOW);
        Serial.println("Engine fan disabled.");
      }
      else if(operation == LO){
        digitalWrite(fan_hi, LOW);
        digitalWrite(fan_lo, HIGH);
        Serial.println("Engine fan set to low.");
      }
      else if(operation == HI){
        digitalWrite(fan_lo, LOW);
        digitalWrite(fan_hi, HIGH);
        Serial.println("Engine fan set to high.");
      }
     break;
     case IAC:
       analogWrite(iac, operation);
       Serial.println("IAC value set.");
     break;
  }
}

void processRead(){
   byte device = Serial.read();
   Serial.flush();
   
   full_sensor_data sensor_data;
   
   switch (device){
    case IAC:
      /* Do not know if this is correct since iac is set to OUTPUT mode. */
      Serial.write(analogRead(iac));
    break;
    case FAN:
      if(bitRead(PORTD, fan_lo)){
        Serial.write(LO);
      }
      else if(bitRead(PORTD, fan_hi)){
        Serial.write(HI);
      }
      else{
        Serial.write(DISABLED);
      }
    break;
    case DME_RELAY:
      Serial.write(bitRead(PORTD, dme_relay));
    break;
    case INJECTOR_RELAY:
      Serial.write(bitRead(PORTD, inj_relay));
    break;
    case FUEL_PUMP_RELAY:
      Serial.write(bitRead(PORTD, fuel_pump));
    break;
    case LAMBDA:
      Serial.write(analogRead(lambda));
    break;
    case AMM:
      Serial.write(amm_conversion(analogRead(amm)));
    break;
    case AMM_TEMP:
      Serial.write(amm_temp_conversion(analogRead(amm_temp)));
    break;
    case ENGINE_TEMP:
      Serial.write(engine_temp_conversion(analogRead(engine_temp)));
    break;
    case RPM:
      Serial.write(RPM);
    break;
    case ALL:
      if(bitRead(PORTD, fan_lo)){
        sensor_data.fan = LO;
      }
      else if(bitRead(PORTD, fan_hi)){
        sensor_data.fan = HI;
      }
      else{
        sensor_data.fan = DISABLED;
      }
      /* Do not know if the following line is correct or not. */
      sensor_data.iac = analogRead(iac);
      sensor_data.dme_relay = bitRead(PORTD, dme_relay);
      sensor_data.fuel_pump = bitRead(PORTD, fuel_pump);
      sensor_data.inj_relay = bitRead(PORTD, inj_relay);
      sensor_data.lambda = analogRead(lambda);
      sensor_data.amm = amm_conversion(analogRead(amm));
      sensor_data.amm_temp = amm_temp_conversion(analogRead(amm_temp));
      sensor_data.rpm = RPM;
      sensor_data.engine_temp = engine_temp_conversion(analogRead(engine_temp));
      sensor_data.fuel_consumption = 0;
      Serial.write((uint8_t*)&sensor_data, sizeof(full_sensor_data));
    break; 
   }
}

float engine_temp_conversion(int engine_temp){
 /* Simple table lookup. */
 return 0; 
}

float amm_conversion(int amm){
  /* Simple table lookup. */
  return 0;
}

float amm_temp_conversion(int amm_temp){
  /* Simple table lookup. */
  return 0;
}
