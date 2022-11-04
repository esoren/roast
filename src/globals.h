#ifndef GLOBALS_H
#define GLOBALS_H

//see globals.cpp for variable explanations 

extern int fanspeed; 

extern const float fan_min_duty;
extern const float fan_max_duty; 

extern unsigned int pidWindowSize;


extern double heatOnTime;

extern double   pidPwrOutput;  

extern double pwrSetpoint;
extern double pidSetpoint;

extern int onboardTemp;
extern double chamberTemp;
extern int ambientTemp;

extern unsigned long windowStartTime;

extern const double aggKp;
extern const double aggKi;
extern const double aggKd;

extern const double consKp;
extern const double consKi;
extern const double consKd;

extern const double aggConsThresh;


extern int heatMode; 


extern int curveStartTemp;
extern int curveEndTemp;
extern int curveEndTime;
extern int curveBend;
extern int curveRampTime;


extern int MODE;


extern int onboard_thermistor_pin;

extern Adafruit_MCP9600 mcp; 
extern PID myPID;
extern EasyNex myNex;

#endif
