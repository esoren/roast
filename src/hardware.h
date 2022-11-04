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

/* PID STATES */
#define PID_MANUAL  0
#define PID_AUTO    1

/* PIN DEFINITIONS */
#define fan_in1_pin 9 //used for fan PWM control
#define fan_in2_pin 10
#define fan_enable_pin 2
#define heat_enable_pin 4 
#define thermocouple_pwr_pin 7

/* DEVICE CONFIGS */ 
#define MCP_I2C_ADDRESS (0x67)

void setup_fan();
void set_fan_speed(int speed);
void enable_fan();
void disable_fan();
int is_fan_on();
void setup_heat();
void set_heater_output_manual(float heater_percent);
void initialize_pid(double init_setpoint);
void disable_heater();
void set_pid_mode(int pid_mode);
void run_pid();
int read_onboard_temp();
void setup_thermocouple();
double read_thermocouple_temp();
double read_ambient_temp();

#endif 
