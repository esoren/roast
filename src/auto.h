#ifndef AUTO_H
#define AUTO_H

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
