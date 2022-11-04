#ifndef CONFIG_H
#define CONFIG_H

extern const float fan_min_duty;
extern const float fan_max_duty; 

extern unsigned int pidWindowSize;

#define NEXTION_BAUD 115200 //needs to be configured to match in the nextion .hmi file 

#define THERMOCOUPLE_LPF_ALPHA 0.6 //LPF coefficient for the thermocouple. 0.0 - 1.0. Lower values are less responsive and smoother. 

#define FAN_MIN_DUTY_CYCLE 0.7 //0.7 * 24V
#define FAN_MAX_DUTY_CYCLE 0.99 //0.99 * 24V

#define PID_MAX_TEMP 250 // max target temperature in degrees C


#endif
