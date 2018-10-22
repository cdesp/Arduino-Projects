#include "Arduino.h"
#include "DspServo.h"

DESP_Servo Servo_0;
 
void setup() {
  int a=max(15,5);
  Servo_0.init(15);
}

void loop() {
   
   Servo_0.servoLeftAt(5);
   Servo_0.servoLeft(11);
   Servo_0.servoRight(3);
   Servo_0.servoRightAt(4);
}
