#ifndef STATE_H
#define STATE_H

extern int heatMode; 

void enter_mode(int newmode);
void update_mode(int event);
int get_current_mode();
/* STATE EVENTS */

#define EVENT_FAN_BUTTON_ON          0
#define EVENT_FAN_BUTTON_OFF         1
#define EVENT_HEAT_BUTTON_ON         2
#define EVENT_HEAT_BUTTON_OFF        3
#define EVENT_PID_BUTTON_ON          4
#define EVENT_PID_BUTTON_OFF         5
#define EVENT_AUTO_BUTTON_ON         6
#define EVENT_AUTO_BUTTON_OFF        7

#endif
