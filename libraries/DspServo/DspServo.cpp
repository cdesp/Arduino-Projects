#include "DspServo.h"



DESP_Servo::DESP_Servo() {
  srvpos=90;
}

void DESP_Servo::init(int pin){
  serv.attach(pin);
  serv.write(srvpos);//goto
}

void DESP_Servo::servoLeftAt(int deg) {	
	srvpos=max(90-deg,0);
	serv.write(srvpos);	
}

void DESP_Servo::servoRightAt(int deg){
  srvpos=min(90+deg,180);
  serv.write(srvpos);		
}
void DESP_Servo::servoLeft(int deg){
   srvpos=max(srvpos-deg,0);
   serv.write(srvpos);	
}
void DESP_Servo::servoRight(int deg){
   srvpos=min(srvpos+deg,180);
   serv.write(srvpos);		
}
