#include "CPRobot.h"


CP_Robot::CP_Robot() {
 distTravelled=0;
 
}
void CP_Robot::init(int m1pin1,int m1pin2,int m2pin1,int m2pin2,int myphotointpin,int mygyroaddr=MPU6050_DEFAULT_ADDRESS)
{
  Motor1=new DESP_DCMotor;    
  Motor1->init(1,m1pin1,m1pin2);
  Motor2=new DESP_DCMotor;
  Motor2->init(2,m2pin1,m2pin2);
  dspGyro=new DESP_Gyro(mygyroaddr);
  dspGyro->init(); //Calibrates also
  photopin=myphotointpin;
  //hook photo pin interrupt  
  
}
void CP_Robot::handleInterrupt()
{
  if (robostate==rcForward)  distTravelled++;
  if (robostate==rcBackward) distTravelled--;
}
void CP_Robot::setStepTicks(int myticks)
{
  stepTicks=myticks;
}
void CP_Robot::setNewstate(ROBOSTATE rs)
{
  laststate=robostate;
  robostate=rs;
}
void CP_Robot::runForward()
{
  distTravelled=0; 
  setNewstate(rcForward); 
  Motor1->run(FORWARD);
  Motor2->run(FORWARD);
}
void CP_Robot::runBackward()
{
  distTravelled=0;
  setNewstate(rcBackward); 
  Motor1->run(BACKWARD);
  Motor2->run(BACKWARD);  
}
void CP_Robot::runStop()
{
  setNewstate(rcStop);  
  Motor1->run(RELEASE);
  Motor2->run(RELEASE);  
}
void CP_Robot::stepForward()
{
  if (laststate!=robostate)
    distTravelled=0;
  Motor1->run(FORWARD);
  Motor2->run(FORWARD);
  setNewstate(rcForwStep);
}
void CP_Robot::stepBackward()
{
  if (laststate!=robostate)
    distTravelled=0;
  Motor1->run(BACKWARD);
  Motor2->run(BACKWARD);  
  setNewstate(rcBackStep);
}
void CP_Robot::turnRight(int deg)
{
  setNewstate(rcRight); 
  dspGyro->goRight(deg);
  goRight();
}
void CP_Robot::turnLeft(int deg) 
{
  setNewstate(rcLeft); 
  dspGyro->goLeft(deg);
  goLeft();
}
void CP_Robot::goRight()
{
  setNewstate(rcRight); 
  Motor1->run(BACKWARD); //right motor
  Motor2->run(FORWARD);  //left motor  
}
void CP_Robot::goLeft() 
{
  setNewstate(rcLeft); 
  Motor1->run(FORWARD); //right motor
  Motor2->run(BACKWARD);  //left motor     
}
int CP_Robot::curDegrees()
{
 return dspGyro->curbearing; 
}
int CP_Robot::targDegrees()
{
 return dspGyro->targbearing; 
}
boolean CP_Robot::isTargetReached()
{
 return  !dspGyro->turning;
}
void CP_Robot::roboloop()
{
  //check if we reached destination turning or moving
  switch(robostate)
  {
   case rcForwStep:if (distTravelled>=stepTicks){
                    distTravelled-=stepTicks;
                    runStop();
                   }
                   break;
   case rcBackStep:if (distTravelled<=-stepTicks){
                    distTravelled+=stepTicks;
                    runStop();
                   }
                   break;
  }
  if (!isTargetReached()){
    dspGyro->getSafeGyroPos();//safe but slow
    if (dspGyro->isTargetReached(maxdeviation)){  //Are we there yet?                                 
     runStop();        //could be off so we should recheck if problems occur
     dspGyro->targetReached(); 
    } else { //not there yet
     int dis=dspGyro->getDistance();//how far away we are
     if ((lastturndistance<0 && dis>0) || (lastturndistance>0 && dis<0) ) {//if we overdoit
      runStop(); 
      if (dis<0) goLeft(); else goRight();
     }
     lastturndistance=dis;
    }
  }

  
}

