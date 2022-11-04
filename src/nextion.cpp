
#include "EasyNextionLibrary.h"
#include <PID_v1.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include "Adafruit_MCP9600.h"
#include "nextion.h"
#include "hardware.h"
#include "state.h"
#include "globals.h"
#include "defines.h"


/* LIST OF EACH NEXTION TRIGGER CALLBACK */
//trigger0  - request from nextion for mcu to update temp and fan values
//trigger1  - fanSlider Touch Move
//trigger2  - fanBtn OFF
//trigger3  - fanBtn ON
//trigger4  - heatBtn OFF
//trigger5  - heatBtn ON
//trigger6  - pidBtn OFF
//trigger7  - pidBtn ON
//trigger8  - autoBtn OFF
//trigger9  - autoBtn ON
//trigger10 - heatSlider Touch Move 
//trigger11 - calculate auto curve
//trigger12 - update auto temp 


//Write MCU global variables to Nextion display  
//Triggered by mcuUpdateTimer every 500ms
//printh 23 02 54 00
void trigger0() {
  myNex.writeNum("chamberTemp", chamberTemp); 
  myNex.writeNum("ambientTemp", ambientTemp);
  myNex.writeNum("onboardTemp", onboardTemp);
  


  if(heatMode==HEATMODE_PID) {
    myNex.writeNum("pwrDisplay", (int)float(pidPwrOutput)*2.5);
    myNex.writeStr("pwrText.txt", String(pidPwrOutput, 0));  
    myNex.writeNum("pidDisplay", pidSetpoint); 
  } else if (heatMode==HEATMODE_PWR) {
    myNex.writeNum("pwrDisplay", (int)float(pwrSetpoint)*2.5);   
    myNex.writeNum("pidDisplay", (int)float(pwrSetpoint)*2.5); //hide PID curve under power curve
    myNex.writeStr("pidText.txt", "n/a");
  }
  

  
  
  myNex.writeNum("fanspeed", fanspeed); 
  return;
}

// fanSpeed slider has been updated, ask Nextion for the new value 
//Triggered by fanSlider Touch Move event 
//printh 23 02 54 01
void trigger1() {

  
  double newFanspeed = myNex.readNumber("fanSlider.val");
  if(newFanspeed != 777777) { //777777 = error flag from easyNex library
    if(newFanspeed <= 100 && newFanspeed >= 0) {
      fanspeed = newFanspeed;
    }
  }
  myNex.writeStr("fanText.txt", String(fanspeed));
  set_fan_speed(fanspeed);
  return;
}

// heatSpeed slider has been updated, ask Nextion for the new value 
//Triggered by heatSlider Touch Move event 
//printh 23 02 54 0A
void trigger10() {

  double newHeatSliderValue = myNex.readNumber("heatSlider.val");
  if(newHeatSliderValue != 777777) { //777777 = error flag from easyNex library
    
    if(heatMode==HEATMODE_PWR) {
      
      if(newHeatSliderValue >= 0 && newHeatSliderValue <= 100) { //todo: make the ranges #define values 
        pwrSetpoint = newHeatSliderValue;
        myNex.writeStr("pwrText.txt", String(pwrSetpoint, 0));
      }
    } else if (heatMode==HEATMODE_PID) {
      if(newHeatSliderValue >= 1 && newHeatSliderValue <= 250) {
        pidSetpoint = newHeatSliderValue;
        myNex.writeStr("pidText.txt", String(pidSetpoint, 0));
      }
    }
  }
  
  if(MODE==4) {
    set_heater_output_manual(pwrSetpoint);
  }
  
  return;
}



void set_button_enable(int button, int enable) {
    // button = FANBTN, HEATBTN, PIDBTN, or AUTOBTN define
    // enable = 0 to disable button, enable = 1 to enable button 

    switch(button) {
      case FANBTN:
        if(enable) { 
            myNex.writeNum("fanBtn.pco",  TXT_ENABLE_COLOR);
            myNex.writeNum("fanBtn.pco2", TXT_ENABLE_COLOR);
            myNex.writeNum("fanBtn.bco",  FANBTN_OFF_COLOR);
            myNex.writeNum("fanBtn.bco2", FANBTN_ON_COLOR);
        } else {
            myNex.writeNum("fanBtn.pco",  TXT_DISABLE_COLOR);
            myNex.writeNum("fanBtn.pco2", TXT_DISABLE_COLOR);
            myNex.writeNum("fanBtn.bco",  FANBTN_OFF_COLOR);
            myNex.writeNum("fanBtn.bco2", FANBTN_OFF_COLOR);
        }
        break;
      
      case HEATBTN:
        if(enable) { 
            myNex.writeNum("page0.heatBtn.pco",  TXT_ENABLE_COLOR);
            myNex.writeNum("page0.heatBtn.pco2", TXT_ENABLE_COLOR);
            myNex.writeNum("page0.heatBtn.bco",  HEATBTN_OFF_COLOR);
            myNex.writeNum("page0.heatBtn.bco2", HEATBTN_ON_COLOR);
        } else {
            myNex.writeNum("page0.heatBtn.pco",  TXT_DISABLE_COLOR);
            myNex.writeNum("page0.heatBtn.pco2", TXT_DISABLE_COLOR);
            myNex.writeNum("page0.heatBtn.bco",  HEATBTN_OFF_COLOR);
            myNex.writeNum("page0.heatBtn.bco2", HEATBTN_OFF_COLOR);
        }
        break;
        
      case PIDBTN:
        if(enable) { 
            myNex.writeNum("pidBtn.pco",  TXT_ENABLE_COLOR);
            myNex.writeNum("pidBtn.pco2", TXT_ENABLE_COLOR);
            myNex.writeNum("pidBtn.bco",  PIDBTN_OFF_COLOR);
            myNex.writeNum("pidBtn.bco2", PIDBTN_ON_COLOR);
        } else {
            myNex.writeNum("pidBtn.pco",  TXT_DISABLE_COLOR);
            myNex.writeNum("pidBtn.pco2", TXT_DISABLE_COLOR);
            myNex.writeNum("pidBtn.bco",  PIDBTN_OFF_COLOR);
            myNex.writeNum("pidBtn.bco2", PIDBTN_OFF_COLOR);
        }
        break;
    
      case AUTOBTN:
        if(enable) { 
            myNex.writeNum("autoBtn.pco",  TXT_ENABLE_COLOR);
            myNex.writeNum("autoBtn.pco2", TXT_ENABLE_COLOR);
            myNex.writeNum("autoBtn.bco",  AUTOBTN_OFF_COLOR);
            myNex.writeNum("autoBtn.bco2", AUTOBTN_ON_COLOR);
        } else {
            myNex.writeNum("autoBtn.pco",  TXT_DISABLE_COLOR);
            myNex.writeNum("autoBtn.pco2", TXT_DISABLE_COLOR);
            myNex.writeNum("autoBtn.bco",  AUTOBTN_OFF_COLOR);
            myNex.writeNum("autoBtn.bco2", AUTOBTN_OFF_COLOR);
        }
        break;
    
    }
  
}


void set_button_state(int button, int state) {
  if(state > 1) {
    state = 1;
  }
  switch(button) {
    case FANBTN:
      myNex.writeNum("fanBtn.val", state);
      break;
    case HEATBTN:
      myNex.writeNum("heatBtn.val", state);
      break;
    case PIDBTN:
      myNex.writeNum("pidBtn.val", state);
      break;
    case AUTOBTN:
      myNex.writeNum("autoBtn.val", state);
      break;
      
  }
  return;
}


void set_setpoint_mode(int new_setpoint_mode) {
  if(new_setpoint_mode == SETPOINT_PWR) {
    myNex.writeNum("heatSlider.minval", 0);
    myNex.writeNum("heatSlider.maxval", 100);
    myNex.writeNum("heatSlider.pco", 65519);
    myNex.writeNum("heatPlusBtn.bco", 65519);
    myNex.writeNum("heatMinusBtn.bco", 65519); //todo, make these color values #defines 
    myNex.writeNum("heatSlider.val", pwrSetpoint);
    myNex.writeStr("pwrText.txt", String(pwrSetpoint,0));
    myNex.writeStr("pidText.txt", "n/a");
    myNex.writeNum("powerMode", 0); //the nextion needs to know which mode is set so that the slider updates the appropriate value
    
    heatMode=HEATMODE_PWR;
      
  } else if (new_setpoint_mode == SETPOINT_PID) {
    myNex.writeNum("heatSlider.minval", 1);
    myNex.writeNum("heatSlider.maxval", 250);
    myNex.writeNum("heatSlider.pco", 34800);
    myNex.writeNum("heatPlusBtn.bco", 34800);
    myNex.writeNum("heatMinusBtn.bco", 34800);
    myNex.writeNum("heatSlider.val", pidSetpoint);
    myNex.writeStr("pidText.txt", String(pidSetpoint,0));
    myNex.writeNum("powerMode", 1); //the nextion needs to know which mode is set so that the slider updates the appropriate value
    
    heatMode=HEATMODE_PID; 
    
  }
  
  return;
}

//fanBtn Touch Turned Off Event
//printh 23 02 54 02
void trigger2() {
  update_mode(EVENT_FAN_BUTTON_OFF);
  return;
}

//fanBtn Touch Turned On Event
//printh 23 02 54 03
void trigger3() {
  update_mode(EVENT_FAN_BUTTON_ON);
  return;
}

//heatBtn Touch Turned Off Event
//printh 23 02 54 04
void trigger4() {
  update_mode(EVENT_HEAT_BUTTON_OFF);
  return;
}

//heatBtn Touch Turned On Event
//printh 23 02 54 05
void trigger5() {
  update_mode(EVENT_HEAT_BUTTON_ON);
  return;
}

//pidBtn Touch Turned Off Event
//printh 23 02 54 06
void trigger6() {
  update_mode(EVENT_PID_BUTTON_OFF);
  return;
}

//pidBtn Touch Turned On Event
//printh 23 02 54 07
void trigger7() {
  update_mode(EVENT_PID_BUTTON_ON);
  return;
}

//autoBtn Touch Turned Off Event
//printh 23 02 54 08
void trigger8() {
  update_mode(EVENT_AUTO_BUTTON_OFF);
  return;
}

//autoBtn Touch Turned On Event
//printh 23 02 54 09
void trigger9() {
  update_mode(EVENT_AUTO_BUTTON_ON);
  return;
}
