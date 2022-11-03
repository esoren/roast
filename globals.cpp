


#include "EasyNextionLibrary.h"
#include <PID_v1.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include "Adafruit_MCP9600.h"
#include "defines.h"
#include "globals.h"

int fanspeed = 100; //setting for the fan speed, updated when the user adjusts the fan speed slider 

const float fan_min_duty = 0.70*255; //minimum duty cycle of the fan is 70%. This corresponds to fan speed = 1
const float fan_max_duty = 0.99*255; //max duty cycle = 99% 

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

//aggressive PID constants are used when the chamber temp is far from the pidSetpoint [abs(error) > aggConsThresh]
const double aggKp=120;
const double aggKi=30;
const double aggKd=60;

//conservative PID constants are used when the chamber temp is near the pidSetpoint [abs(error) < aggConsThresh] 
const double consKp=70;
const double consKi=15;
const double consKd=10;

const double aggConsThresh=10;

PID myPID(&chamberTemp, &heatOnTime, &pidSetpoint, aggKp, aggKi, aggKd, DIRECT);

int heatMode = HEATMODE_PWR; //0 = PID disabled (PWR mode), 1 = PID enabled (PID mode)

//These curve values are used for auto operation. They are retrieved from the Nextion at the start of curve calculation. 
int curveStartTemp = 0; //degrees C
int curveEndTemp = 0;   //degrees C
int curveEndTime = 0;   //seconds
int curveBend = 0;      //degrees C
int curveRampTime = 0;  //seconds (time to reach the peak of the bend )


int MODE = 0; //this indicates which mode the device is in. Fan only, Fan+Heat, etc. See state diagram: "roast_state.drawio.png" for more info. 

int onboard_thermistor_pin = A0; //analog input pin used to read the onboard thermistor 

Adafruit_MCP9600 mcp; //mcp object for reading from the thermocouple 

EasyNex myNex(Serial); //object for communicating with Nextion display 
