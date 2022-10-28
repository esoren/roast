#include "EasyNextionLibrary.h"
#include <PID_v1.h>

EasyNex myNex(Serial);
#include "hardware.h"
#include "nextion.h"
#include "state.h"
 



void setup() {

  

  myNex.begin(115200); //setup serial connection to Nextion Display. todo: can this be 115200?

  setup_fan(); //setup fan pins, leave fan off 
  setup_heat(); //setup heater pins, leave heater off 
  
  set_fan_speed(100); //set fan speed to 100, but does NOT enable fan 
  
  set_button_enable(FANBTN, 1);
  set_button_enable(HEATBTN, 1);
  set_button_enable(PIDBTN, 1);
  set_button_enable(AUTOBTN, 0);

  myNex.writeStr("fanText.txt", "50");
  

  setup_pid_timer();
  initialize_pid(0); //initialize pid with a default setpoint of 40c. Leave in manual mode by default
  set_pid_timer_enable(1);
  disable_heater(); //heater should be off by default, but confirm here


  setup_thermocouple();
  
  enter_mode(0);

  
}



void loop() {
   myNex.NextionListen();
   
}
