#ifndef STATE_H
#define STATE_H

extern int heatMode; 
extern int MODE;

void enter_mode(int newmode);
void update_mode(int event);

#endif
