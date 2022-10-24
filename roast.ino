#include "EasyNextionLibrary.h"
#include <PID_v1.h>

EasyNex myNex(Serial);


/*GLOBAL SETUP
 * 
 */
 
//inputs to the motor controlled used for PWM signal
int in1_pin = 9;
int in2_pin = 10;

int fan_enable_pin = 2;
int heat_enable_pin = 4; 

int thermistorPin = A0;
double onboardTemp = 25;

int fan_setval = 1;
int temp_setval = 25;

const float min_duty = 0.70*255;
const float max_duty = 0.99*255;

// params for PID control 
#define RelayPin 4
double pidOutput, pidSetpoint;
int pidWindowSize = 2500; //period in ms for relay PWM 
unsigned long windowStartTime;
PID myPID(&onboardTemp, &pidOutput, &pidSetpoint, 2, 5, 1, DIRECT);


/* CALLBACKS FROM NEXTION DISPLAY
 *  
 */

//ENABLE FAN
void trigger0() { 
  enable_fan();
  return;
}

//DISABLE FAN
void trigger1(){
  disable_heater(); 
  disable_fan();
  return;
}

//ADJUST FAN SPEED
void trigger2() { 
  fan_setval = myNex.readNumber("h0.val");
  set_fan_speed(fan_setval);
}

//ENABLE HEATER
void trigger3() {
  if(is_fan_on()) {
    enable_heater();
  }
}

//DISABLE HEATER
void trigger4() {
  disable_heater();
}

//REQUEST TEMP UPDATE 
void trigger5() {
  myNex.writeNum("onboardTemp", onboardTemp);
  return;
}

//REQUEST SETPOINT UPDATE
void trigger6() {
  temp_setval = myNex.readNumber("h1.val");
  if(temp_setval > 150)
    temp_setval = 150;
  if(temp_setval < 1)
    temp_setval = 1; 
    
  pidSetpoint = temp_setval;
}

/* HARDWARE INPUT/OUTPUT
 *  
 */

int is_fan_on() {
  return digitalRead(fan_enable_pin);
}
 
void enable_fan() {
  digitalWrite(fan_enable_pin, 1);
  return;
}

void disable_fan() {
  digitalWrite(fan_enable_pin, 0);
  return;
}

void enable_heater() {
  myPID.SetMode(AUTOMATIC);
  return;
}

void disable_heater() {
  myPID.SetMode(MANUAL);
  digitalWrite(heat_enable_pin, 0);
  return;
}

void set_fan_speed(int speed) {
  //speed is set from 0-100 pct of operating speed

  if(speed > 100)
    speed = 100;

  if(speed < 1)
    speed = 1;

  float calc = float(speed)/100*(max_duty-min_duty)+min_duty;
  int val = int(calc);

  //write PWM registers directly to avoid issue with AnalogWrite(255) being treated as a DigitalWrite()  
  OCR1A = val;
  OCR1B = val;

  return;
}


void setup() {
  myNex.begin(9600);
  
  pinMode(in1_pin, OUTPUT);
  pinMode(in2_pin, OUTPUT);
  digitalWrite(fan_enable_pin, 0);
  pinMode(fan_enable_pin, OUTPUT);

  digitalWrite(heat_enable_pin, 0);
  pinMode(heat_enable_pin, OUTPUT);
  
    
  TCCR1A = 0b10110000 | (TCCR1A & 0b00001111); //configure the PWM lines to be complimentary for H-bridge driver 

  set_fan_speed(100);

   //setup timer2 interrupt on a 10ms period 
   cli(); //disable interrupts
   TCCR2A = 0;
   TCCR2B = 0;
   TCCR2B |= B00000111;        //Set CS20, CS21 and CS22 to 1 so we get prescalar 1024
   TIMSK2 |= B00000100;        //Set OCIE1B to 1 so we enable compare match B
   OCR2B = 156;                //Finally we set compare register B to this value -- set for approx 10ms period 
   sei();                      //Enable back the interrupts

   windowStartTime = millis();
   pidSetpoint = 50; 
   myPID.SetOutputLimits(0, pidWindowSize);
   myPID.SetMode(MANUAL);
}

int Vo;
float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

void loop() {
   myNex.NextionListen();
 

   
    
    
    
    
 
}


//TIMER2 INTERRUPT
ISR(TIMER2_COMPB_vect){                               

      static int isr_count = 0;

      
      
      //Estimate  onboard temp using thermistor
      Vo = analogRead(thermistorPin);
      R2 = R1 * (1023.0 / (float)Vo - 1.0);
      logR2 = log(R2);
      T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
      T = T - 273.15;
      onboardTemp=T;
      //todo: average T 
      
      isr_count = (isr_count + 1) % 20; 
      if(isr_count == 0) {
        myPID.Compute();
        unsigned long now = millis();
        if (now - windowStartTime > pidWindowSize)
        { //time to shift the Relay Window
          windowStartTime += pidWindowSize;
        }
        if (pidOutput > now - windowStartTime) {
          digitalWrite(RelayPin, HIGH);
        } else {
          digitalWrite(RelayPin, LOW);
        }

      }
      return;
}
