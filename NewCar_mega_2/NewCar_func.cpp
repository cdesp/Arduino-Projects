#ifndef newcarfunc_h
#define newcarfunc_h

#include "NewCar_mega.h"
#include <DspSensors.h>
#include <DspRobot.h>


void showinfo1();
void showinfo0();



boolean obstleft(){
  boolean ret;

  servo.write(0);
  delay(200);
  Mysensors.checkStatus();
  showinfo1(); 
  BTSerial.println(Mysensors.distanceFront());
  ret= !Mysensors.frontIsClear(); 
  servo.write(90);
  return ret;
}

boolean obstright(){
  boolean ret;

  servo.write(180);
  delay(200);
  Mysensors.checkStatus();
  showinfo1(); 
  BTSerial.println(Mysensors.distanceFront());
  ret= !Mysensors.frontIsClear(); 
  servo.write(90);
  return ret;
}

void checkobstacle(){
  
  Mysensors.checkStatus();

  if (!Mysensors.frontIsClear() && (Myrobo.RoboState()==rcForward)) {
    Myrobo.Stop(true);

    Serial.println("Stopped not clear ahead!!!");  
  } 
  if (Mysensors.frontBumperHit()){

    Myrobo.Stop(true);
    Myrobo.stepBackward();
  } 
}

//Information 
void showinfo1(){

  lcd.setCursor(0, 1);
  lcd.print("               ");
  lcd.setCursor(0, 1);

  lcd.print("d:");
  lcd.setCursor(3, 1);  
  lcd.print(Mysensors.distanceFront());

  //  if (Mysensors.obstacleLeft()) {
  //    lcd.setCursor(10, 1);      
  //    lcd.print("L");  
  //  }

  //  if (Mysensors.obstacleRight()) {
  //    lcd.setCursor(12, 1);  
  //    lcd.print("R");  
  //  }


  // lcd.setCursor(12, 1);  
  // lcd.print(Myrobo.getCurrentCommand());  
}

String smode;

void showinfo0(){
  lcd.setCursor(0, 0);
  lcd.print("                 ");
  lcd.setCursor(0, 0);
  if (Err!="") 
    lcd.print(Err);     
  else { 
    lcd.print(smode);
    lcd.setCursor(6, 0); 
    //   if (comnd!="")
    //     Serial.println("CMND:["+comnd+"]");   
    lcd.print(comnd);
    lcd.setCursor(12, 0);    
    lcd.print(lastcmd);
    if (Mysensors.blLine!=blNone){
      lcd.setCursor(14, 0);    
      lcd.print("BLN");

    }

  }
}

void showBLinfo(){
  if (Myrobo.robomode==robotbl)
    switch (Mysensors.blLine){

    case blNone:
      //BTSerial.println("No BlackLine");
      break;
    case blLeft:
      BTSerial.println("black Left");
      break;
    case blRight:
      BTSerial.println("black Right");
      break;
    case blFarLeft:
      BTSerial.println("black Far Left");
      break;
    case blFarRight:
      BTSerial.println("black Far Right");
      break;    
    case blFront:
      BTSerial.println("Black Front");
      break;


    } 

}




#endif
