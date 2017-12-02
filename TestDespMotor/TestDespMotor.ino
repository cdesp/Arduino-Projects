#include <DspMotor.h>
#include "SoftwareSerial.h" 

#define Motpin1 11
#define Motpin2 12

DESP_DCMotor Motor1;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Serial Ok");
  Motor1.init(1,Motpin1,Motpin2);
  Serial.println("motor1 init");
}

int s;
int cmd;
void loop() {
  // put your main code here, to run repeatedly:
  s=Serial.read();
  cmd=0;
  if (s=='q')  
   cmd=1;//forw
  if (s=='a')
   cmd=2;//Backw
  if (s==' ')
   cmd=4;//Stop

  if (cmd>0){
     Motor1.run(cmd);
     Serial.println(cmd);
  }
   
}
