#ifndef AUTO_H
#define AUTO_H

/* MEMORY SETUP FOR NEXTION CURVE DISPLAY UPDATE */
//These values can be adjusted to save memory at the cost of curve calculation time. 
//The product of the two values should equal 220. 
//Using 11 pages with length 20 caused too much memory consumption 
//Using 44 pages with length 5 could be used if more memory is needed in the future. 
//If these are changed it will also require changes to writeCurveDispToNextion and 
//the nextion .hmi file curve update timer event. 

#define PAGENUM_LENGTH 10
#define NUMPAGES 22

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

#endif
