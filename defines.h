
/* STATE MACHINE DEFINES */

#define HEATMODE_PWR 0
#define HEATMODE_PID 1


/* PIN DEFINITIONS */

#define fan_in1_pin 9 //used for fan PWM control
#define fan_in2_pin 10
#define fan_enable_pin 2
#define heat_enable_pin 4 
#define thermocouple_pwr_pin 7

/* DEVICE CONFIGS */ 

#define MCP_I2C_ADDRESS (0x67)

#define THERMOCOUPLE_LPF_ALPHA 0.6 //LPF coefficient for the thermocouple. 0.0 - 1.0. Lower values are less responsive and smoother. 


/* NEXTION BUTTONS AND COLORS */

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

/* STATE EVENTS */

#define EVENT_FAN_BUTTON_ON          0
#define EVENT_FAN_BUTTON_OFF         1
#define EVENT_HEAT_BUTTON_ON         2
#define EVENT_HEAT_BUTTON_OFF        3
#define EVENT_PID_BUTTON_ON          4
#define EVENT_PID_BUTTON_OFF         5
#define EVENT_AUTO_BUTTON_ON         6
#define EVENT_AUTO_BUTTON_OFF        7

/* POWER MODES */

#define SETPOINT_PWR 0
#define SETPOINT_PID 1

/* MEMORY SETUP FOR NEXTION CURVE DISPLAY UPDATE */
#define PAGENUM_LENGTH 10
#define NUMPAGES 22
