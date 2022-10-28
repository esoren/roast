
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include "Adafruit_MCP9600.h"

/* ****************************** FUNCTION PROTOTYPES *****************************  */

void setup_fan();
void set_fan_speed(int speed);
void enable_fan();
void disable_fan();
int is_fan_on();

void setup_heat();
void set_pid_timer_enable(int timer_enabled);
void setup_pid_timer();
void set_heater_output_manual(float heater_percent);
void initialize_pid(double init_setpoint);

void disable_heater();


int read_onboard_temp();
void set_button_enable(int button, int enable);

void setup_thermocouple();
double read_thermocouple_temp();
double read_ambient_temp();

/* ********************************  PIN DEFINITIONS  ********************************* */

//inputs to the fan motor controller used for PWM signal
int fan_in1_pin = 9;
int fan_in2_pin = 10;
int fan_enable_pin = 2;

int heat_enable_pin = 4; 

int onboard_thermistor_pin = A0;
int thermocouple_pwr_pin = 7;


/* ******************************** GLOBALS ***************************************** */

int fanspeed = 100; //setting for the fan speed, updated when the user adjusts the fan speed slider 

unsigned int pidWindowSize = 2500; //period in ms for relay PWM 


double heatOnTime = 0; //when PID is enabled this value is written by reference
                       //when PID is disabled this value can be written manually
                       //This should be between 0 and pidWindowSize  
                       //If this is set to pidWindowSize the heater will always be on 
                       
double setpoint = 50;    //Target for PID OR percent duty cycle of heater, depending on mode 

double onboardTemp = 25; //temperature of the onboard thermistor. This is updated inside of the PID timer interrupt routine
double chamberTemp = 25; //temperature measured by a type-K thermocouple inside of the heating chamber. 
double ambientTemp = 25; //temperature measured at the cold junction of the MCP9600

unsigned long windowStartTime; //this is updated as part of the PID processing to handle heat PWM duty cycle 

double Kp=2;
double Ki=5;
double Kd=2;
PID myPID(&chamberTemp, &heatOnTime, &setpoint, Kp, Ki, Kd, DIRECT);

int pidMode = 0; //0 = PID disabled, 1 = PID enabled
int pidSetpointCache = 50; //used to store the previous PID setpoint value when switching to PWR mode
int pwrSetpointCache = 0; //used to store the previous PWR setpoint value when switching to PWR mode

/* ************************************  CONFIG  ************************************* */

const float fan_min_duty = 0.70*255; 
const float fan_max_duty = 0.99*255;



/* **********************************  FAN CONTROL  *********************************** */

void setup_fan() {

  pinMode(fan_in1_pin, OUTPUT);
  pinMode(fan_in2_pin, OUTPUT);
  digitalWrite(fan_enable_pin, 0);
  pinMode(fan_enable_pin, OUTPUT);
  TCCR1A = 0b10110000 | (TCCR1A & 0b00001111); //configure the PWM lines to be complimentary for H-bridge driver 
  

  return;
}

void set_fan_speed(int speed) {
  //speed is set from 0-100 pct of operating speed

  if(speed > 100)
    speed = 100;

  if(speed < 1)
    speed = 1;

  fanspeed = speed;  //update global variable 
  
  float calc = float(speed)/100*(fan_max_duty-fan_min_duty)+fan_min_duty;
  int val = int(calc);

  //write PWM registers directly to avoid issue with AnalogWrite(255) being treated as a DigitalWrite()  
  OCR1A = val;
  OCR1B = val;

  return;
}

void enable_fan() {
  digitalWrite(fan_enable_pin, 1);
  return;
}

void disable_fan() {
  digitalWrite(fan_enable_pin, 0);
  return;
}

int is_fan_on() {
  return digitalRead(fan_enable_pin);
}


/* **********************************  HEAT/PID CONTROL  *********************************** */

void setup_heat() {
  digitalWrite(heat_enable_pin, 0);
  pinMode(heat_enable_pin, OUTPUT);
  return;
}

void set_pid_timer_enable(int timer_enabled) {
  
  cli(); //disable interrupts
  if(timer_enabled) {
    TIMSK2 |= B00000100;        //Set OCIE1B to 1 so we enable compare match B
  } else {
    TIMSK2 &= B11111011;        //Clear OCIE1B so we disable compare match B
  }
  sei();                      //Enable back the interrupts
  return;
}

void setup_pid_timer() {
  //setup timer2 interrupt on a ~10ms period 
  //will call ISR(TIMER2_COMPB_vect) every timer period 
   cli(); //disable interrupts
   TCCR2A = 0;
   TCCR2B = 0;
   TCCR2B |= B00000111;        //Set CS20, CS21 and CS22 to 1 so we get prescalar 1024
   OCR2B = 156;                //Finally we set compare register B to this value -- set for approx 10ms period 
   set_pid_timer_enable(0);    //Don't enable the timer by default
   sei();                      //Enable back the interrupts
}


void set_heater_output_manual(float heater_percent) {
  
  heatOnTime = (double)((heater_percent/100)*pidWindowSize);
  if(heatOnTime > pidWindowSize) {
    heatOnTime = pidWindowSize;
  }
  return;
}


void initialize_pid(double init_setpoint) {
   windowStartTime = millis();
   setpoint = init_setpoint; 
   myPID.SetOutputLimits(0, pidWindowSize);
   myPID.SetMode(MANUAL);
   set_heater_output_manual(0);
   return;
}



void disable_heater() {
  myPID.SetMode(MANUAL);
  set_heater_output_manual(0);
  return;
}


ISR(TIMER2_COMPB_vect){                               
      //This interrupt routine is configured by setup_pid_timer()
      static int isr_count = 0;

      //Measure temperatures 
      onboardTemp=read_onboard_temp(); //todo: average this reading (inside of temp routine or here?)
      chamberTemp=read_thermocouple_temp();
      ambientTemp=read_ambient_temp();
      
      //Process PID loop every 20 interrupt calls, manage duty cycle of heater 
      isr_count = (isr_count + 1) % 20; 
      
      if(isr_count == 0) {
        myPID.Compute();
        unsigned long now = millis();
        if (now - windowStartTime > pidWindowSize)
        { //time to shift the Relay Window
          windowStartTime += pidWindowSize;
        }
        
        if (heatOnTime > now - windowStartTime) {
          digitalWrite(heat_enable_pin, 1);
        } else {
          digitalWrite(heat_enable_pin, 0);
        }

      }
      return;
}


  


/* **********************************  TEMP READBACK  *********************************** */

//read the onboard temp from the thermistor mounted under the heater element 
int read_onboard_temp() {
  
  int Vo;
  const float R1 = 10000;
  const float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
  float logR2, R2, T;

  //Estimate  onboard temp using thermistor
  Vo = analogRead(onboard_thermistor_pin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;

  return T; 
}


/* *********************************** THERMOCOUPLE  ********************************* */

#define MCP_I2C_ADDRESS (0x67)

Adafruit_MCP9600 mcp;



void setup_thermocouple() {

    
   mcp.begin(MCP_I2C_ADDRESS);
   mcp.setADCresolution(MCP9600_ADCRESOLUTION_18);
   mcp.setThermocoupleType(MCP9600_TYPE_K);
   mcp.enable(true);
      

   return;
}

double read_thermocouple_temp() {
  double tempval = mcp.readThermocouple();
  return tempval;
}

double read_ambient_temp() {
  double tempval = mcp.readAmbient();
  return tempval;
}
