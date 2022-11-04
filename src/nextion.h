#ifndef NEXTION_H 
#define NEXTION_H

extern EasyNex myNex;

/* POWER MODES */
#define HEATMODE_PWR 0
#define HEATMODE_PID 1

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

void trigger0();
void trigger1();
void trigger10();
void set_button_enable(int button, int enable);
void set_button_state(int button, int state);
void set_setpoint_mode(int new_setpoint_mode);
void write_mode_to_display(int MODE);
void trigger2();
void trigger3();
void trigger4();
void trigger5();
void trigger6();
void trigger7();
void trigger8();
void trigger9();

#endif
