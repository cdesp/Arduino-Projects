#ifndef newcarloops_h
#define newcarloops_h

#include "NewCar_mega.h"
#include <DspSensors.h>
#include <DspRobot.h>
#include "NewCar_func.cpp"





//continuous movement
void RCloop(){

  checkobstacle();

  switch (lastcmd){
  case 'w':
    if (Mysensors.frontIsClear())
      Myrobo.Forward();              
    break;
  case 's':
    Myrobo.Backward();
    break;
  case ' ':
  case '*':
    Myrobo.Stop(true);
    break;
  case 'a':
    Myrobo.left();
    break;
  case 'd':
    Myrobo.right();
    break;       
  }  
}


//Step movement
String mes1;

void RCloop2(){

  checkobstacle();

  mes1=Myrobo.checkTimetoStop();//checks for movement timeout
  if (mes1!="") {
    Serial.println("MSG:"+mes1);   
  }


  switch (lastcmd){
  case 'w':
    if (Mysensors.frontIsClear())
      Myrobo.stepForward();              
    break;
  case 's':
    Myrobo.stepBackward();
    break;
  case ' ':
  case '*':    
    Myrobo.Stop(true);
    break;
  case 'a':
    Myrobo.turnLeft(45);
    break;
  case 'd':
    Myrobo.turnRight(45);
    break;

  }


}


//Robot self guidance


void Robotloop(){

  //   BTSerial.println("robo turn Start ======");
  //  BTSerial.println("checkobst");
  checkobstacle();

  if (Myrobo.RoboState()==rcStop) { //check for black line
    //  BTSerial.println("rcStop");
    if ((Myrobo.robomode!=robot) && (Mysensors.blLine !=blNone)){
      switch (Mysensors.blLine){
      case blLeft: 
        Myrobo.turnLeft(15);
        break;
      case blFarLeft: 
        Myrobo.turnLeft(35);
        break;
      case blRight: 
        Myrobo.turnRight(15);
        break;
      case blFarRight: 
        Myrobo.turnRight(35);
        break;            
      case blFront: 
        if (Mysensors.frontIsClear()){
          Err="";
          Myrobo.stepForward();
        }
        break;

      } 

    }
    else
    {
      // BTSerial.println("check Moving");
      if (Mysensors.frontIsClear()){
        Err="";
        Myrobo.stepForward();
      }
      else
      {
        //   BTSerial.println("Stop check left right");
        Myrobo.Stop(true);
        if (!obstleft()) Myrobo.turnLeft(20);
        else
          if (!obstright()) Myrobo.turnRight(20);
          else {
            Err="No way Out. Halted.";  
            Myrobo.stepBackward();          
          }
        //     BTSerial.println(Mysensors.distanceFront());  
      }//frontisclear

    }//bln!=blnone
  }//moving=0
  Mysensors.checkStatus();
  // BTSerial.println("robo turn end======");
  //  BTSerial.println("");
}


#endif
