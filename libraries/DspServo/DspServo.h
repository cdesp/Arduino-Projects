#ifndef DspServo_H
  #define DspServo_H
  
#include <Servo.h>
#include "Arduino.h"

class DESP_Servo
{
 public:
  DESP_Servo();
  void init(int pin);
  void servoLeftAt(int deg);
  void servoRightAt(int deg);
  void servoLeft(int deg);
  void servoRight(int deg);
  
    
 private:  
 	Servo serv; //MicroServo SG90
 	int srvpos;
}; 	

  
  
#endif  