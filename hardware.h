
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
      //the run_pid name is a bit of a misnomer, because this runs in both PID mode and PWR mode. 
      //this should run on a regular basis whenever the heater is enabled. 
      // in PID mode the PID is configured for automatic operation and automatically sets the heatOnTime variable. 
      // in PWR mode the PID is configured for manual mode and heatOnTime is set by user input. This function still handles the duty cycle of the motor relay. 
      
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

void setup_thermocouple() {
   mcp.begin(MCP_I2C_ADDRESS);
   mcp.setADCresolution(MCP9600_ADCRESOLUTION_18);
   mcp.setThermocoupleType(MCP9600_TYPE_K);
   mcp.enable(true);
   double tempval = mcp.readThermocouple();
   return;
}


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
