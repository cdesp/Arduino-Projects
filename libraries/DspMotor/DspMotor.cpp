#include "DspMotor.h"






DESP_DCMotor::DESP_DCMotor() {

  
  
}

void DESP_DCMotor::init(int mymotorno,int mypin1,int mypin2) {
	motorno=mymotorno;
  cpin1=mypin1;
  cpin2=mypin2;
  
  pinMode(cpin1, OUTPUT);
  pinMode(cpin2, OUTPUT);
	run(RELEASE);

}


void DESP_DCMotor::run(int cmd) {
  
  switch (cmd) {
  case FORWARD:
        digitalWrite(cpin1, LOW); // set pin 2 on L293D low
        digitalWrite(cpin2, HIGH); // set pin 7 on L293D high    
     //   dwrite(cpin1, LOW); // set pin 2 on L293D low
     //   dwrite(cpin2, HIGH); // set pin 7 on L293D high    
     
    break;
  case BACKWARD:
        digitalWrite(cpin2, LOW); // set pin 7 on L293D low    
        digitalWrite(cpin1, HIGH); // set pin 2 on L293D high        
   //     dwrite(cpin1, HIGH); // set pin 2 on L293D low
   //     dwrite(cpin2, LOW); // set pin 7 on L293D high    

    break;
  case RELEASE:
        digitalWrite(cpin1, LOW); // set pin 2 on L293D low
        digitalWrite(cpin2, LOW); // set pin 7 on L293D low    
   //     dwrite(cpin1, LOW); // set pin 2 on L293D low
   //     dwrite(cpin2, LOW); // set pin 7 on L293D high    

    break;
  }
}