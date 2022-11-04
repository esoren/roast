#ifndef HARDWARE_H
#define HARDWARE_H


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
