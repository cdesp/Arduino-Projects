



#ifndef DspMotor_H
  #define DspMotor_H
  //#include <Wire.h>    // Required for I2C communication
 // #include <PCF8575.h> // Required for PCF8575
 // #include <DspExpander.h>;
 #include "Arduino.h"


#define FORWARD 1
#define BACKWARD 2
//#define BRAKE 3
#define RELEASE 4




class DESP_DCMotor
{
 public:
  DESP_DCMotor();
  void init(int mymotorno,int mypin1,int mypin2);
  void run(int cmd);


 private:
  int motorno,cpin1,cpin2;
};

#endif