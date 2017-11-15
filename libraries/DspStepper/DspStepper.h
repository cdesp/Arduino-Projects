
#ifndef DspStepper_H
 #define DspStepper_H

 //#include <DspExpander.h>
 #include "Arduino.h"


class DESP_Stepper
{
 public:
 	
  DESP_Stepper();
  void init(int sm1,int sm2,int sm3,int sm4,int whatspeed);
  void DoTurn(int Steps);
  

  
 private: 	
 
 	int steps_left;
	boolean Direction;// gre
	int IN1;
  int IN2;
  int IN3;
  int IN4;
  int Steps;
  int Speed;
 
 	void stepper(int xw);
 	void SetDirection();
  
  
};




#endif