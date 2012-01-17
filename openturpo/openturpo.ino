
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
  FAN_LO =          'k',
  FAN_HI =          'l'
}DEVICE;

enum{
  ON =   'a',
  OFF =  'b'
}OPERATION;

/* This value is for E85.
 * For unleaded gasoline, use 14.7
 */
static float afr = 9.76;

/* The following is a table of the density of pure
 * ethanol at various temperatures in centigrade and g/cc.
 */
float fuel_dens_avg = 0.802;
float fuel_density[32][32] = {
  {
    3,4,5,6,7,8,9,10,11,12,16,17,18,19,20,21,22,23,24,25,29,30,31,32,33,34,35,36,37,38,39,40  }
  ,
  {
    0.8037,0.80290,0.80207,0.80123,0.80039,0.79956,0.79872,
    0.79788,0.79704,0.79620,0.79283,0.79198,0.79114,0.79029,
    0.78945,0.78860,0.78775,0.78691,0.78606,0.78522,0.78182,
    0.78097,0.78012,0.77927,0.77841,0.77756,0.77671,0.77585,
    0.77500,0.77414,0.77329,0.77244}
};

/* This value represents the injector flow rates.
 * It is in the units of cc/min @ 3 bar of pressure.
 */
float inj = 350;
float inj_hrs = inj*60;
float inj_hrs_mass = inj_hrs*fuel_dens_avg/1000;

int cylinders = 6;

int amm = A0;
int amm_temp = A1;
int lambda = A2;
int engine_temp = A4;
int injector_bank_one = 4;
int injector_bank_two = 5;
int cmp = 2;
int crk = 3;
int iac = 6;
int ign_1 = 22;
int ign_2 = 23;
int ign_3 = 24;
int ign_4 = 25;
int ign_5 = 26;
int ign_6 = 27;
int fuel_pump = 28;
int inj_relay = 29;
int dme_relay = 30;
int fan_lo = 30;
int fan_hi = 31;

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
  
  /* Set up serial communication. */
  Serial.begin(9600);
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
   */
}

void crank_trigger(){
  /* In this function we use the trigger pulses from
   * the crankshaft to continuously update engine
   * RPM.
   */
  static unsigned long last_time = 0;
  unsigned long current_time = micros();
  unsigned long time_diff = current_time - last_time;
  static float RPM = 60000000/time_diff;
}

void serialEvent(){
  /* This function handles communication with the outside world
   * via the serial communication interface.
   */
   byte command = Serial.read();
   Serial.println("Command received");
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
     case FAN_LO:
      if(operation == ON){
        digitalWrite(fan_hi, LOW);
        digitalWrite(fan_lo, HIGH);
        Serial.println("Engine fan set to low speed.");
      }
      else{
        digitalWrite(fan_lo, LOW);
        digitalWrite(fan_hi, LOW);
        Serial.println("Engine fan offline.");
      }
     break;
     case FAN_HI:
      if(operation == ON){
        digitalWrite(fan_lo, LOW);
        digitalWrite(fan_hi, HIGH);
        Serial.println("Engine fan set to high speed.");
      }
      else{
        digitalWrite(fan_lo, LOW);
        digitalWrite(fan_hi, LOW);
        Serial.println("Engine fan offline.");
      }
     break;
  }
}

void processRead(){
  
}

