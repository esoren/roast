#include "EasyNextionLibrary.h"
#include <PID_v1.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include "Adafruit_MCP9600.h"
#include "src/defines.h"
#include "src/config.h"
#include "src/hardware.h"
#include "src/nextion.h"
#include "src/state.h"
#include "src/auto.h"

void setup() {

  
  myNex.begin(115200); //setup serial connection to Nextion Display. todo: can this be 115200?

  setup_fan(); //setup fan pins, leave fan off 
  setup_heat(); //setup heater pins, leave heater off 
  
  set_fan_speed(100); //set fan speed to 100, but does NOT enable fan 
  
  set_button_enable(FANBTN, 1);
  set_button_enable(HEATBTN, 1);
  set_button_enable(PIDBTN, 1);
  set_button_enable(AUTOBTN, 0);
  
  
  initialize_pid(0); //initialize pid with a default setpoint of 40c. Leave in manual mode by default

  disable_heater(); //heater should be off by default, but confirm here
  
  setup_thermocouple();
  
  enter_mode(0); //enter state machine for UI control 
  
}


void loop() {
   myNex.NextionListen();

   chamberTemp=read_thermocouple_temp();
   ambientTemp=read_ambient_temp();

   run_pid();
   delay(100);
}
