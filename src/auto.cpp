#include "EasyNextionLibrary.h"
#include <PID_v1.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include "Adafruit_MCP9600.h"
#include "auto.h"
#include "config.h"
#include "hardware.h"
#include "nextion.h"
#include "state.h"

//These curve values are used for auto operation. They are retrieved from the Nextion UI at the start of curve calculation. 
int curveStartTemp = 0; //degrees C
int curveEndTemp = 0;   //degrees C
int curveEndTime = 0;   //seconds
int curveBend = 0;      //degrees C
int curveRampTime = 0;  //seconds (time to reach the peak of the bend )

//Receive request from nextion display to calculate the display curve and update display values 
//Triggered by page1 calcButton
//printh 23 02 54 0B
void trigger11() {
  unsigned char displayWaveform [PAGENUM_LENGTH];
  static int pageNum = 0;
  int currentTimeInSeconds = 0;
  double newCalc = myNex.readNumber("newCalc"); //this is true if the calculate button was just pushed 
  
  if(newCalc == 1 || newCalc == 777777) { //if the button is first pushed start a new calculation. If a bad read from mcu, restart calculation 
    //the first time this function is called, retrieve each of the slider values, calculate the curve, and write page0
    curveStartTemp  = myNex.readNumber("startSlider.val");
    curveEndTemp    = myNex.readNumber("endSlider.val");
    curveEndTime    = myNex.readNumber("autoTimeInSeconds");
    curveBend       = myNex.readNumber("curveBendVal");
    curveRampTime   = myNex.readNumber("rampSlider.val");

    //todo: add some error checking for reasonableness of values (is either start or end temp below ambient?)
    
    myNex.writeStr("page1.statusText.txt", "calculating..");
    pageNum = 0;
    writeAndConfirmNumber("newCalc", 0); //clear the newCalc flag so future calls to this function just write
                                         //the next page to memory without reseting the calculation 
  } 
    
  if(pageNum == NUMPAGES) { //Is the final page update complete?
    myNex.writeNum("calcTimer.en", 0);
    myNex.writeStr("statusText.txt", "calculation complete"); 
    myNex.writeNum("curveDisp.gdc", 33808);
    myNex.writeNum("curveDisp.pco0", 64512);
    myNex.writeNum("calcProgress.val", 0); //only show the status display when calculation is being performed 
    myNex.writeNum("autoSetup", 1); //used to indicate that auto setup has been completed at least once 
    pageNum=0;
  } else {
  
    for(int x=0; x<PAGENUM_LENGTH; x++) {
      currentTimeInSeconds = int(float(pageNum*PAGENUM_LENGTH+x)/(NUMPAGES*PAGENUM_LENGTH)*curveEndTime);
                                // = current pixel / total pixels * curveEndTime 
      displayWaveform[x] = calculateAutoTemp(currentTimeInSeconds);
        
      }
    writeCurveDispToNextion(displayWaveform);
    myNex.writeNum("calcProgress.val", int(float(pageNum)/(NUMPAGES-1)*100));
    pageNum++;
  }
  
}

void writeAndConfirmNumber(String variableToWrite, unsigned char value) {
  unsigned char readback_value;
  myNex.writeNum(variableToWrite, value);

  readback_value = myNex.readNumber(variableToWrite);
  while(readback_value != value) {
    myNex.writeNum(variableToWrite, value);
    readback_value = myNex.readNumber(variableToWrite);
  }
  return;
}

//this unrolled for loop is required because the nextion doesn't allow user-defined arrays
void writeCurveDispToNextion (unsigned char displayWaveform[]) {
  writeAndConfirmNumber("curveDisp0", displayWaveform[0]);
  writeAndConfirmNumber("curveDisp1", displayWaveform[1]);
  writeAndConfirmNumber("curveDisp2", displayWaveform[2]);
  writeAndConfirmNumber("curveDisp3", displayWaveform[3]);
  writeAndConfirmNumber("curveDisp4", displayWaveform[4]);
  writeAndConfirmNumber("curveDisp5", displayWaveform[5]);
  writeAndConfirmNumber("curveDisp6", displayWaveform[6]);
  writeAndConfirmNumber("curveDisp7", displayWaveform[7]);
  writeAndConfirmNumber("curveDisp8", displayWaveform[8]);
  writeAndConfirmNumber("curveDisp9", displayWaveform[9]);  
  //more lines must be added here if the number of pages is reduced
 
  myNex.writeNum("mcuFinishedUpdatingDisplay", 1); 
  
  return;
}
  
//uses these globals:
//int curveStartTemp;
//int curveEndTemp;
//int curveEndTime;
//int curveBend;
//int curveRampTime;

unsigned char calculateAutoTemp(int currentTimeInSeconds) {
  
  float slope, currentTemp, bendSlope, bend; 
  unsigned char currentTempOutput;

  // first calculate without the bend 
  slope = float(curveEndTemp-curveStartTemp)/curveEndTime;
  currentTemp = curveStartTemp + slope*currentTimeInSeconds;

  //calculate the bend value 
  if(currentTimeInSeconds == curveRampTime) {
        bend = curveBend;
  } else if (currentTimeInSeconds < curveRampTime) {
        bendSlope = float(curveBend)/curveRampTime;
        bend = bendSlope*currentTimeInSeconds;
  } else {
        bendSlope = float(curveBend)/(curveEndTime-curveRampTime);
        bend = bendSlope*(curveEndTime-currentTimeInSeconds);
  }

  //combine the bend and the 
  currentTemp += bend; 
  currentTempOutput = currentTemp;
  //todo: add post error checking (e.g. is the temperature negative or below ambient?)
  return currentTempOutput;
}

void enableAutoTimer() {
  myNex.writeNum("autoTimer.en", 1);
}

void disableAutoTimer() {
  myNex.writeNum("autoTimer.en", 0);
  setAutoProgress(0);
}

void setAutoTimerSeconds(int secs) {
  myNex.writeNum("autoSeconds", 0);
  setAutoProgress(0);
}

void initAutoTimer() {
  pidSetpoint= curveStartTemp;
  myNex.writeStr("pidText.txt", String(pidSetpoint, 0));
  myNex.writeNum("heatSlider.val", pidSetpoint);
  myNex.writeNum("pidDisplay", pidSetpoint); //todo: can combine all of these items into one function to set PID values on Nextion 
  setAutoProgress(0);
  setAutoTimerSeconds(0);
}

//receive request from autotimer to update PID auto temperature 
//Triggered by autoTimer on nextion display 
//printh 23 02 54 0C

void trigger12() {
  int autoSeconds = myNex.readNumber("autoSeconds");

  if(autoSeconds >= curveEndTime) { //auto finished
    pidSetpoint = calculateAutoTemp(curveEndTime); //calculate and set to the final value 
    myNex.writeStr("pidText.txt", String(pidSetpoint, 0));
    myNex.writeNum("heatSlider.val", pidSetpoint);
    myNex.writeNum("pidDisplay", pidSetpoint); 
    setAutoProgress(0);
    update_mode(EVENT_AUTO_BUTTON_OFF); //set auto button to off and revert back to PID mode at the final temperature 
                                        //todo: consider turning off heat and setting fan to max.. 
  } else {
    pidSetpoint = calculateAutoTemp(autoSeconds);
    myNex.writeStr("pidText.txt", String(pidSetpoint, 0));
    myNex.writeNum("heatSlider.val", pidSetpoint);
    myNex.writeNum("pidDisplay", pidSetpoint);  
    setAutoProgress(autoSeconds);
  }
  
}

void setAutoProgress(int currentTimeInSeconds) {
    int progressPct = int(float(currentTimeInSeconds)/curveEndTime*100.0);
    myNex.writeNum("page0.autoProgress.val", progressPct);
    return;
}
