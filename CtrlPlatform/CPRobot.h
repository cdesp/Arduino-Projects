#ifndef CPRobot_H
 #define CPRobot_H

#include <DspMotor.h>
#include <DspGyro.h>

#define FORWARD 1
#define BACKWARD 2
#define RELEASE 4

#define maxdeviation 3


enum ROBOSTATE {
  rcBackward,
  rcForward,
  rcStop,
  rcForwStep,
  rcBackStep,
  rcLeft,
  rcRight  
};

typedef void (*attintfunction) () ;

class CP_Robot
{
 private:
  int photopin;
  volatile int distTravelled; //in ticks
  int stepTicks;
  int lastturndistance;
  ROBOSTATE robostate;
  ROBOSTATE laststate;
  DESP_DCMotor* Motor1;
  DESP_DCMotor* Motor2;
  DESP_Gyro* dspGyro;
 public:
  CP_Robot();
  void init(int m1pin1,int m1pin2,int m2pin1,int m2pin2,int myphotointpin,int mygyroaddr=MPU6050_DEFAULT_ADDRESS);
  void setStepTicks(int myticks);
  void roboloop();
  void setNewstate(ROBOSTATE rs);
  int curDegrees();
  int targDegrees();
  boolean isTargetReached();
  void handleInterrupt();
  void run(int cmd);
  void runForward();
  void runBackward();
  void runStop();
  void stepForward();
  void stepBackward();
  void turnRight(int deg);
  void turnLeft(int deg);  
  void goRight();  
  void goLeft();
  boolean isRoboIdle();
 };




#endif
