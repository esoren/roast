#ifndef HARDWARE_H
#define HARDWARE_H


extern int fanspeed; 

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

extern int onboard_thermistor_pin;

void setup_fan();
void set_fan_speed(int speed);
void enable_fan();
void disable_fan();
int is_fan_on();
void setup_heat();
void set_heater_output_manual(float heater_percent);
void initialize_pid(double init_setpoint);
void disable_heater();
void run_pid();
int read_onboard_temp();
void setup_thermocouple();
double read_thermocouple_temp();
double read_ambient_temp();





#endif 
