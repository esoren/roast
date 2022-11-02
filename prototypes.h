
/* auto.h */

void trigger11();
void writeAndConfirmNumber(String variableToWrite, unsigned char value);
void writeCurveDispToNextion (unsigned char displayWaveform[]);
unsigned char calculateAutoTemp(int currentTimeInSeconds);
void enableAutoTimer();
void disableAutoTimer();
void setAutoTimerSeconds(int secs);
void initAutoTimer();
void trigger12();
void setAutoProgress(int currentTimeInSeconds);

/* hardware.h */
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

/* nextion.h */

void trigger0();
void trigger1();
void trigger10();
void set_button_enable(int button, int enable);
void set_button_state(int button, int state);
void set_setpoint_mode(int new_setpoint_mode);
void trigger2();
void trigger3();
void trigger4();
void trigger5();
void trigger6();
void trigger7();
void trigger8();
void trigger9();

/* state.h */

void enter_mode(int newmode);
void update_mode(int event);
