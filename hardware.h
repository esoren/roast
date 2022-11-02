
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include "Adafruit_MCP9600.h"



#define HEATMODE_PWR 0
#define HEATMODE_PID 1

/* ****************************** FUNCTION PROTOTYPES *****************************  */

void setup_fan();
void set_fan_speed(int speed);
void enable_fan();
void disable_fan();
int is_fan_on();

void setup_heat();

void set_heater_output_manual(float heater_percent);
void initialize_pid(double init_setpoint);

void disable_heater();


int read_onboard_temp();
void set_button_enable(int button, int enable);

void setup_thermocouple();
double read_thermocouple_temp();
double read_ambient_temp();

void run_pid();
/* ********************************  PIN DEFINITIONS  ********************************* */

//todo: can these be #defines to save memory?
#define fan_in1_pin 9 //used for fan PWM control
#define fan_in2_pin 10
#define fan_enable_pin 2
#define heat_enable_pin 4 
int onboard_thermistor_pin = A0;
#define thermocouple_pwr_pin 7


/* ******************************** GLOBALS ***************************************** */

int fanspeed = 100; //setting for the fan speed, updated when the user adjusts the fan speed slider 

unsigned int pidWindowSize = 3600; //period in ms for relay PWM 


double heatOnTime = 0; //when PID is enabled this value is written by reference
                       //when PID is disabled this value can be written manually
                       //This should be between 0 and pidWindowSize  
                       //If this is set to pidWindowSize the heater will always be on 

double   pidPwrOutput = 0; //percent of power enabled when PID is running.
                        //This is used to update the display
                        //= heatOnTime/pidWindowSize*100
                        // this is updated in the PID routine 
                       
double pwrSetpoint = 0;    //power level in PWR mode 0-100%
double pidSetpoint = 50;   //pid setpoint 1-250C

int onboardTemp = 25; //temperature of the onboard thermistor. This is updated inside of the PID timer interrupt routine
double chamberTemp = 25; //temperature measured by a type-K thermocouple inside of the heating chamber. 
int ambientTemp = 25; //temperature measured at the cold junction of the MCP9600

unsigned long windowStartTime; //this is updated as part of the PID processing to handle heat PWM duty cycle 

const double aggKp=120;
const double aggKi=30;
const double aggKd=60;

const double consKp=70;
const double consKi=15;
const double consKd=10;

const double aggConsThresh=10;

PID myPID(&chamberTemp, &heatOnTime, &pidSetpoint, aggKp, aggKi, aggKd, DIRECT);


int heatMode = HEATMODE_PWR; //0 = PID disabled (PWR mode), 1 = PID enabled (PID mode)

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




void set_heater_output_manual(float heater_percent) {
  
  heatOnTime = (double)((heater_percent/100)*pidWindowSize);
  if(heatOnTime > pidWindowSize) {
    heatOnTime = pidWindowSize;
  }
  return;
}


void initialize_pid(double init_setpoint) {
   windowStartTime = millis();
   pidSetpoint = init_setpoint; 
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


void run_pid() {                             
      //This interrupt routine is configured by setup_pid_timer()
      onboardTemp = read_onboard_temp(); 

      //todo: the section of code below can be set to run only when PID is enabled       
      double gap = abs(chamberTemp-pidSetpoint); //distance away from setpoint
      if (gap < aggConsThresh) {  
        myPID.SetTunings(consKp, consKi, consKd); //we're close to setpoint, use conservative tuning parameters
      } else {
        myPID.SetTunings(aggKp, aggKi, aggKd); //we're far from setpoint, use aggressive tuning parameters
      }
      
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

      pidPwrOutput = int(float(heatOnTime)/float(pidWindowSize)*100);

    
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
   double tempval = mcp.readThermocouple();
   return;
}


#define THERMOCOUPLE_LPF_ALPHA 0.6

//uses a single pole LPF to smooth out the temp data 

double read_thermocouple_temp() {
  static bool first_run=1; //initialize the filter on the first run 
  
  static double prevOutput;
  static double filteredOutput=25; 
  double currentMeasurement = 25;
  
  prevOutput=filteredOutput;
  currentMeasurement = mcp.readThermocouple();
  
  if(first_run==1) {
    prevOutput=currentMeasurement;
    first_run=0;
  }
  
  filteredOutput = THERMOCOUPLE_LPF_ALPHA*currentMeasurement + (1-THERMOCOUPLE_LPF_ALPHA)*prevOutput; 
  
  return currentMeasurement;
}

double read_ambient_temp() {
  double tempval = mcp.readAmbient();
  return tempval;
}
