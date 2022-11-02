


void enter_mode(int newmode) {
  switch(newmode) {
    case 0:
          myPID.SetMode(MANUAL);
          disable_heater();
          disable_fan();
          
          set_setpoint_mode(SETPOINT_PWR);
          
          set_button_enable(  FANBTN,   1);
          set_button_enable(  HEATBTN,  0);
          set_button_enable(  PIDBTN,   1);
          set_button_enable(  AUTOBTN,  0);
          
          set_button_state(   FANBTN,   0);
          set_button_state(   PIDBTN,   0);

          disableAutoTimer();

          MODE=0;
          break;


    case 1:
          myPID.SetMode(MANUAL);
          disable_heater();
          disable_fan();
          
          set_setpoint_mode(SETPOINT_PID);
          
          set_button_enable(  FANBTN,   1);
          set_button_enable(  HEATBTN,  0);
          set_button_enable(  PIDBTN,   1);
          set_button_enable(  AUTOBTN,  0);
          
          set_button_state(   FANBTN,   0);
          set_button_state(   PIDBTN,   1);

          disableAutoTimer();
          
          MODE=1;
          break;


    case 2: 
          myPID.SetMode(MANUAL);
          disable_heater();
          enable_fan();
          
          set_setpoint_mode(SETPOINT_PWR);
          
          set_button_enable(  FANBTN,   1);
          set_button_enable(  HEATBTN,  1);
          set_button_enable(  PIDBTN,   1);
          set_button_enable(  AUTOBTN,  0);
          
          set_button_state(   FANBTN,   1);
          set_button_state(   HEATBTN,  0);
          set_button_state(   PIDBTN,   0);

          disableAutoTimer();
          
          MODE=2;
          break;

     case 3: 
          myPID.SetMode(MANUAL);
          disable_heater();
          enable_fan();
          
          set_setpoint_mode(SETPOINT_PID);
          
          set_button_enable(  FANBTN,   1);
          set_button_enable(  HEATBTN,  1);
          set_button_enable(  PIDBTN,   1);
          set_button_enable(  AUTOBTN,  0);
          
          set_button_state(   FANBTN,   1);
          set_button_state(   HEATBTN,  0);
          set_button_state(   PIDBTN,   1);

          disableAutoTimer();
          
          MODE=3;
          break;

    case 4: 
          myPID.SetMode(MANUAL);
          set_setpoint_mode(SETPOINT_PWR);
          enable_fan();
          set_heater_output_manual(pwrSetpoint);
          
          set_button_enable(  FANBTN,   1);
          set_button_enable(  HEATBTN,  1);
          set_button_enable(  PIDBTN,   1);
          set_button_enable(  AUTOBTN,  0);
          
          set_button_state(   FANBTN,   1);
          set_button_enable(  HEATBTN,  1);
          set_button_state(   PIDBTN,   0);

          disableAutoTimer();
          
          MODE=4;
          break;

     case 5: 
          set_setpoint_mode(SETPOINT_PID);
          enable_fan();
          myPID.SetMode(AUTOMATIC);
          
          
          set_button_enable(  FANBTN,   1);
          set_button_enable(  HEATBTN,  1);
          set_button_enable(  PIDBTN,   1);
          set_button_enable(  AUTOBTN,  1);
          
          set_button_state(   FANBTN,   1);
          set_button_enable(  HEATBTN,  1);
          set_button_state(   PIDBTN,   1);
          set_button_state(   AUTOBTN,  0);

          disableAutoTimer();
          
          MODE=5;
          break;
     
      case 6:
          set_setpoint_mode(SETPOINT_PID);
          enable_fan();
          myPID.SetMode(AUTOMATIC);
          
          
          set_button_enable(  FANBTN,   1);
          set_button_enable(  HEATBTN,  1);
          set_button_enable(  PIDBTN,   1);
          set_button_enable(  AUTOBTN,  1);
          
          set_button_state(   FANBTN,   1);
          set_button_enable(  HEATBTN,  1);
          set_button_state(   PIDBTN,   1);
          set_button_state(   AUTOBTN,  1);

          setAutoTimerSeconds(0);
          initAutoTimer();
          enableAutoTimer();
          
          
          
          MODE=6;
          break;
    
  }

  myNex.writeStr("modeText.txt", String(MODE));
}




void update_mode(int event) {
  switch (MODE) {
    case 0:
      switch (event) {
        case EVENT_FAN_BUTTON_ON:
          enter_mode(2);
          break;
        case EVENT_PID_BUTTON_ON:
          enter_mode(1);
          break;
      }
      break;
    
    case 1:
      switch (event) {
        case EVENT_PID_BUTTON_OFF:
          enter_mode(0);
          break;
        case EVENT_FAN_BUTTON_ON:
          enter_mode(3);
          break;  
      }
      break;
      
    case 2: 
      switch (event) {
        case EVENT_FAN_BUTTON_OFF:
          enter_mode(0);
          break;
        case EVENT_HEAT_BUTTON_ON:
          enter_mode(4);
          break;
        case EVENT_PID_BUTTON_ON:
          enter_mode(3);
          break;
      }
      break;

    case 3:
      switch (event) {
        case EVENT_FAN_BUTTON_OFF:
          enter_mode(1);
          break;
        case EVENT_HEAT_BUTTON_ON:
          enter_mode(5);
          break;
        case EVENT_PID_BUTTON_OFF:
          enter_mode(2);
          break;
      }
      break;

    case 4:
      switch (event) {
        case EVENT_HEAT_BUTTON_OFF:
          enter_mode(2);
          break;
        case EVENT_PID_BUTTON_ON:
          enter_mode(5);
          break;
        case EVENT_FAN_BUTTON_OFF:
          enter_mode(0);
          break;
      }
      break;

    case 5: 
      switch (event) {
        case EVENT_HEAT_BUTTON_OFF:
          enter_mode(3);
          break;
        case EVENT_PID_BUTTON_OFF:
          enter_mode(4);
          break;
        case EVENT_AUTO_BUTTON_ON:
          enter_mode(6);
          break;
        case EVENT_FAN_BUTTON_OFF:
          enter_mode(1);
          break;
      }
    case 6:
      switch (event) {
        case EVENT_AUTO_BUTTON_OFF:
          enter_mode(5);
          break;
        case EVENT_PID_BUTTON_OFF:
          enter_mode(4);
          break;
        case EVENT_HEAT_BUTTON_OFF:
          enter_mode(3);
          break;
        case EVENT_FAN_BUTTON_OFF:
          enter_mode(1);
          break;
      }
      break;
  }
}
