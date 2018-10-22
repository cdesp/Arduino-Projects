#include <DspMotor.h>
#include "SoftwareSerial.h" 

#define Motpin11 11
#define Motpin12 12
#define Motpin21 9
#define Motpin22 10


DESP_DCMotor Motor1;
DESP_DCMotor Motor2;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Serial Ok");
  Motor1.init(1,Motpin11,Motpin12);
  Serial.println("motor1 init");
  Motor2.init(1,Motpin21,Motpin22);
  Serial.println("motor2 init");
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
     Motor2.run(cmd);
     Serial.println(cmd);
  }
   
}
