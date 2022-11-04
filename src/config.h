#ifndef CONFIG_H
#define CONFIG_H

extern const float fan_min_duty;
extern const float fan_max_duty; 

extern unsigned int pidWindowSize;

extern Adafruit_MCP9600 mcp; 
extern PID myPID;
extern EasyNex myNex;

#endif
