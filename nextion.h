/* LIST OF EACH NEXTION TRIGGER CALLBACK */

extern int MODE; 
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

void update_mode(int event);

//Write MCU global variables to Nextion display  
//Triggered by mcuUpdateTimer every 500ms
//printh 23 02 54 00
void trigger0() {
  myNex.writeNum("chamberTemp", onboardTemp); //todo: set this to chamberTemp once sensor is installed 
  myNex.writeNum("setpoint", setpoint);

  if(pidMode==0) { 
    myNex.writeNum("setpointDisplay", (int)setpoint*2.5);  //in PWR mode, scale the setpoint for full scale in waveform
  } else {
    myNex.writeNum("setpointDisplay", setpoint);
  }
  myNex.writeNum("fanspeed", fanspeed); 
  return;
}

// fanSpeed slider has been updated, ask Nextion for the new value 
//Triggered by fanSlider Touch Move event 
//printh 23 02 54 01
void trigger1() {

  double newFanspeed = myNex.readNumber("fanSlider.val");
  if(newFanspeed != 777777) {
    fanspeed = newFanspeed;
  }
  myNex.writeStr("fanText.txt", String(fanspeed));
  set_fan_speed(fanspeed);
  return;
}

// heatSpeed slider has been updated, ask Nextion for the new value 
//Triggered by heatSlider Touch Move event 
//printh 23 02 54 0A
void trigger10() {

  double newSetpoint = myNex.readNumber("heatSlider.val");
  if(newSetpoint != 777777) {
    setpoint = newSetpoint;
  }
  
  
  myNex.writeStr("setText.txt", String(setpoint, 0));
  
  
  if(MODE==4) {
    set_heater_output_manual(setpoint);
  }

  
  return;
}



  

#define FANBTN 0
#define FANBTN_OFF_COLOR 4
#define FANBTN_ON_COLOR 3000

#define HEATBTN 1
#define HEATBTN_OFF_COLOR 14439
#define HEATBTN_ON_COLOR 63488

#define PIDBTN 2
#define PIDBTN_OFF_COLOR 256
#define PIDBTN_ON_COLOR 2016

#define AUTOBTN 3
#define AUTOBTN_OFF_COLOR 6150
#define AUTOBTN_ON_COLOR 32799

#define TXT_ENABLE_COLOR 65535
#define TXT_DISABLE_COLOR 19017

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

#define SETPOINT_PWR 0
#define SETPOINT_PID 1
void set_setpoint_mode(int setpoint_mode) {
  if(setpoint_mode == SETPOINT_PWR) {
    myNex.writeNum("heatSlider.minval", 0);
    myNex.writeNum("heatSlider.maxval", 100);
    myNex.writeNum("heatSlider.pco", 65519);
    myNex.writeStr("setLabel.txt", "Pwr");
    myNex.writeNum("setLabel.pco", 65519);
    myNex.writeNum("setText.pco", 65519);
    myNex.writeNum("tempDisplay.pco1", 65519);

    if(pidMode==1) { //if switching from PID to PWR
      pidMode=0;
      pidSetpointCache = setpoint;
      setpoint = pwrSetpointCache; 
    }
      
  } else if (setpoint_mode == SETPOINT_PID) {
    myNex.writeNum("heatSlider.minval", 1);
    myNex.writeNum("heatSlider.maxval", 250);
    myNex.writeStr("setLabel.txt", "Set");
    myNex.writeNum("heatSlider.pco", PIDBTN_ON_COLOR);
    myNex.writeNum("setLabel.pco", PIDBTN_ON_COLOR);
    myNex.writeNum("setText.pco", PIDBTN_ON_COLOR);
    myNex.writeNum("tempDisplay.pco1", PIDBTN_ON_COLOR);

    if(pidMode==0) { //if switching from PWR to PID
      pidMode=1;
      pwrSetpointCache = setpoint;
      setpoint = pidSetpointCache; 
    }  

  }


  myNex.writeNum("heatSlider.val", setpoint);
  myNex.writeStr("setText.txt", String(setpoint,0));
  return;
}


#define EVENT_FAN_BUTTON_ON          0
#define EVENT_FAN_BUTTON_OFF         1
#define EVENT_HEAT_BUTTON_ON         2
#define EVENT_HEAT_BUTTON_OFF        3
#define EVENT_PID_BUTTON_ON          4
#define EVENT_PID_BUTTON_OFF         5
#define EVENT_AUTO_BUTTON_ON         6
#define EVENT_AUTO_BUTTON_OFF        7


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