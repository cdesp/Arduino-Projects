/*
   BYJ48 Stepper motor code
   Connect :
   IN1 >> D8
   IN2 >> D9
   IN3 >> D10
   IN4 >> D11
   VCC ... 5V Prefer to use external 5V Source
   Gnd
   written By :Mohannad Rawashdeh
  http://www.instructables.com/member/Mohannad+Rawashdeh/
     28/9/2013
  */

#include <DspStepper.h>



DESP_Stepper::DESP_Stepper() {
  steps_left=4095;
  Direction = true;
}

void DESP_Stepper::init(int sm1,int sm2,int sm3,int sm4,int whatspeed) {
	
  IN1=sm1;
  IN2=sm2;
  IN3=sm3;
  IN4=sm4;
  
   
  pinMode(IN1, OUTPUT); 
  pinMode(IN2, OUTPUT); 
  pinMode(IN3, OUTPUT); 
  pinMode(IN4, OUTPUT); 

  Speed =  whatspeed;
 

}


void DESP_Stepper::DoTurn(int Steps)
{
	unsigned long last_time;
  unsigned long currentMillis ;
  int steps_left=4095;
  long time;

  Direction= Steps>0;
  
	steps_left=abs(Steps);
  while(steps_left>0){
  currentMillis = micros();
  if(currentMillis-last_time>=1000){
    stepper(1); 
    time=time+micros()-last_time;
    last_time=micros();
    steps_left--;
  }
  }
  stepper(8);
}

void DESP_Stepper::stepper(int xw){
  for (int x=0;x<xw;x++){
  switch(Steps){
   case 0:
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, HIGH);
   break; 
   case 1:
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, HIGH);
     digitalWrite(IN4, HIGH);
   break; 
   case 2:
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, HIGH);
     digitalWrite(IN4, LOW);
   break; 
   case 3:
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, HIGH);
     digitalWrite(IN3, HIGH);
     digitalWrite(IN4, LOW);
   break; 
   case 4:
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, HIGH);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);
   break; 
   case 5:
     digitalWrite(IN1, HIGH); 
     digitalWrite(IN2, HIGH);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);
   break; 
     case 6:
     digitalWrite(IN1, HIGH); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);
   break; 
   case 7:
     digitalWrite(IN1, HIGH); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, HIGH);
   break; 
   default:
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);
   break; 
}
SetDirection();
}
} 
void DESP_Stepper::SetDirection(){
if(Direction==1){ Steps++;}
if(Direction==0){ Steps--; }
if(Steps>7){Steps=0;}
if(Steps<0){Steps=7; }
}
