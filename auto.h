



//Receive request from nextion display to calculate the display curve 
//Triggered by page1 calcButton
//printh 23 02 54 0B

#define PAGENUM_LENGTH 20
#define NUMPAGES 11

int curveStartTemp = 0;
int curveEndTemp = 0;
int curveEndTime = 0;
int curveBend = 0;
int curveRampTime = 0; //time to reach the peak of the bend 

void writeCurveDispToNextion (char displayWaveform[]);
void writeAndConfirmNumber(String variableToWrite, char value);

void trigger11() {
  char displayWaveform [PAGENUM_LENGTH];
  static int pageNum = 0;
  int newCalc = myNex.readNumber("newCalc"); //this is true if the calculate button was just pushed 
  if(newCalc == 1) {
    //the first time this function is called, retrieve each of the slider values, calculate the curve, and write page0
    curveStartTemp  = myNex.readNumber("startSlider.val");
    curveEndTemp    = myNex.readNumber("endSlider.val");
    curveEndTime    = myNex.readNumber("timeSlider.val");
    curveBend       = myNex.readNumber("bendSlider.val");
    curveRampTime   = myNex.readNumber("rampSlider.val");
    myNex.writeStr("page1.statusText.txt", "calculating..");
    pageNum = 0;
    writeAndConfirmNumber("newCalc", 0); 
  }


  if(pageNum == NUMPAGES) {
    myNex.writeNum("calcTimer.en", 0);
    myNex.writeStr("statusText.txt", "calculation complete"); 
    myNex.writeNum("curveDisp.gdc", 33808);
    myNex.writeNum("curveDisp.pco0", 33823);
    myNex.writeNum("calcProgress.val", 0); //only show the status display when calculation is being performed 
    pageNum=0;
  } else {
  
    for(int x=0; x<PAGENUM_LENGTH; x++) {
      displayWaveform[x] = pageNum*3;
      if(pageNum==10)
        displayWaveform[x] = 200;
      }
    writeCurveDispToNextion(displayWaveform);
    myNex.writeNum("calcProgress.val", int(float(pageNum)/(NUMPAGES-1)*100));
    pageNum++;
  }
  
  
  
}

void writeAndConfirmNumber(String variableToWrite, char value) {
  char readback_value;
  myNex.writeNum(variableToWrite, value);

  readback_value = myNex.readNumber(variableToWrite);
  while(readback_value != value) {
    myNex.writeNum(variableToWrite, value);
    readback_value = myNex.readNumber(variableToWrite);
  }
  return;
}

//this unrolled for loop is required because the nextion doesn't allow user-defined arrays
void writeCurveDispToNextion (char displayWaveform[]) {
  int delay_time = 10;
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
  writeAndConfirmNumber("curveDisp10", displayWaveform[10]);
  writeAndConfirmNumber("curveDisp11", displayWaveform[11]);
  writeAndConfirmNumber("curveDisp12", displayWaveform[12]);
  writeAndConfirmNumber("curveDisp13", displayWaveform[13]);
  writeAndConfirmNumber("curveDisp14", displayWaveform[14]);
  writeAndConfirmNumber("curveDisp15", displayWaveform[15]);
  writeAndConfirmNumber("curveDisp16", displayWaveform[16]);
  writeAndConfirmNumber("curveDisp17", displayWaveform[17]);
  writeAndConfirmNumber("curveDisp18", displayWaveform[18]);
  writeAndConfirmNumber("curveDisp19", displayWaveform[19]);
  
  myNex.writeNum("mcuFinishedUpdatingDisplay", 1); 
  
  return;
}
  
  

  
  

  
