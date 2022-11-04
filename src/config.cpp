


#include "EasyNextionLibrary.h"
#include <PID_v1.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include "Adafruit_MCP9600.h"
#include "defines.h"
#include "config.h"


const float fan_min_duty = 0.70*255; //minimum duty cycle of the fan is 70%. This corresponds to fan speed = 1
const float fan_max_duty = 0.99*255; //max duty cycle = 99% 



Adafruit_MCP9600 mcp; //mcp object for reading from the thermocouple 

EasyNex myNex(Serial); //object for communicating with Nextion display 
