
#include <Servo.h>
#include "desp_hand.h"



Hand rightHand(8);

void setup() {

  rightHand.Setup(3,5,6,4,7,8);
 
  Serial.begin(115200);
  Serial.println("OK");
}

void gest1(){
 // rightHand.moveFinger(4,fingOpen);
 // rightHand.moveFinger(3,fingOpen);
//  rightHand.moveFinger(2,fingClose);
 // rightHand.moveFinger(1,fingClose);
 // rightHand.moveFinger(0,fingClose); 

  rightHand.setPosfingers(fC,fC,fC,fO,fO);   
}

void gest2(){
  //rightHand.moveFinger(4,fingOpen);
 // rightHand.moveFinger(3,fingOpen);
 // rightHand.moveFinger(2,fingClose);
 // rightHand.moveFinger(1,fingClose);
 // rightHand.moveFinger(0,fingOpen); 

  rightHand.setPosfingers(fO,fC,fC,fO,fO);   
}

void gest3(){
 // rightHand.moveFinger(4,fingOpen);
 // rightHand.moveFinger(3,fingClose);
 // rightHand.moveFinger(2,fingClose);
 // rightHand.moveFinger(1,fingClose);
 // rightHand.moveFinger(0,fingClose);

  rightHand.setPosfingers(fC,fC,fC,fC,fO);         
}

void gest4(){
  rightHand.moveFinger(4,fingClose);
  rightHand.moveFinger(3,fingClose);
  rightHand.moveFinger(2,fingOpen);
  rightHand.moveFinger(1,fingClose);
  rightHand.moveFinger(0,fingClose); 

  //rightHand.setPosfingers(fC,fC,fO,fC,fC);   
}

void allopen(){
 // for (int i=0;i<5;i++)
  // rightHand.moveFinger(i,fingOpen);   
  rightHand.setPosfingers(fO,fO,fO,fO,fO);   

}

void allclose(){
//  for (int i=0;i<5;i++)
 //  rightHand.moveFinger(i,fingClose);   
 rightHand.setPosfingers(fC,fC,fC,fC,fC);   
}



void wristleft(){
 rightHand.moveWrist(180);
}

void wristnorm(){
 rightHand.moveWrist(90);
}


void wristright(){
  rightHand.moveWrist(0);
}


void loop() {

  
  
 allopen();
 wristnorm();
  
   

   while (true) {
    rightHand.movAllfingers();
    if (Serial.available() > 0) {
      // read the incoming byte:
      String s=Serial.readString();
      int turn=s.toInt();
      //  myservo1.write(turn);  delay(15);
      switch(turn){
        case 0:allclose();break;
        case 1:gest1();break;
        case 2:gest2();break;
        case 3:gest3();break;
        case 4:gest4();break;
        case 6:wristleft();break;
        case 7:wristnorm();break;
        case 8:wristright();break;
        default: allopen();
      }
      
     
      
      Serial.println(turn);
    }
   }
}
