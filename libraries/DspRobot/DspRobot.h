 



#ifndef DspRobot_H
  #define DspRobot_H
//  #include <Wire.h>    // Required for I2C communication
//  #include <PCF8575.h> // Required for PCF8575  
  #include <DspMotor.h> //
//  #include <DspExpander.h>;
//  #include <stdio.h>
//  #include <string.h>
//  #include <inttypes.h>
//  #include "Arduino.h"


enum RoboMode {
  progload,
  rc,
  rc2,
  robot,
  robotbl 
};

enum RoboCommand {
  rcBackward,
  rcForward,
  rcStop,
  rcLeft,
  rcRight  
};

typedef boolean (*stopfunction) () ;

class DESP_Robot
{
 public:
  DESP_Robot();
  void init(int enp,int rmot1p1,int rmot1p2,int rmot2p1,int rmot2p2,int rmot3p1,int rmot3p2,int rmot4p1,int rmot4p2);
  void Backward();
  void Forward();
  void Stop(boolean full);
  void left();
  void right();
  String getCurrentCommand();
  void stepForward();
  void stepBackward();
  void turnLeft(int deg);
  void turnRight(int deg);
  void motorFrontLeft(boolean FORW);
  void motorFrontRight(boolean FORW);
  void motorBackLeft(boolean FORW);
  void motorBackRight(boolean FORW);
  String checkTimetoStop();
  boolean isTimeUp(unsigned long v,int maxtime);
  boolean moveIsUp();
  boolean turnIsUp();
  boolean roboTimeIsUp();
  boolean isDistanceReached();
  float inCM(int dist);   
  int distanceNowCM();
  void clearDistance();
  boolean moveIsUpCM();
  RoboCommand RoboState();
  void setNextMode();
  
	RoboMode robomode;
	int stepMoveTime,stepTurnTime;	
  boolean useTimeDistance=true;
  int stepMoveinCm;
  volatile int mydist=0;
  int lastdist=0;
  int distanceTravelled=0;

 private:
  DESP_DCMotor motor1;
  DESP_DCMotor motor2,motor3,motor4;  
 
   RoboCommand rcmd;
   RoboCommand lastrcmd;
   unsigned long  motortime;
   int lastAngle;
   int ENPin;
};

#endif