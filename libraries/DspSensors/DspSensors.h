
#ifndef DspSensors_H
 #define DspSensors_H
 //#include <DspExpander.h>;
// #include <IRremote.h> //IR remote control
 #include <NewPing.h>


  



//IR receiver
extern int RECV_PIN;//IR receive pin should be defined




enum BLLine {
  blFarLeft,
  blLeft,
  blFront,
  blRight,
  blFarRight,
  blNone
};




class DESP_Sensors
{
 public:
 	BLLine blLine;
 	
  DESP_Sensors();
  void init();
  int getIRbutton();
  char getIRcommand();
    
  void setSonicParams(int dmin,int sEvery,int trpin,int ecpin);
  int getSonicDistance();
  int distanceFront();
  boolean frontIsClear();
  boolean obstacleLeft();
  boolean obstacleRight();  
  
  
  void setIRObstacleParams(int obstL,int obstR);
  
  void setBlacklineParams(int bl1,int bl2,int bl3,int bl4,int bl5,int frsens);
  void checkBlackLine();
  boolean frontBumperHit();
  boolean BLfarleft();
  boolean BLleft();
  boolean BLmiddle();
  boolean BLright();
  boolean BLfarright();
  	
  void checkStatus();
  


  
 private:
 	//ir sensor
 	unsigned long lstCheck;//to wait for 1 sec before get ir code
 	
	//sonic sensor
	int trigPin,echoPin;
  int distanceMin,distanceLast,sonicEvery;
  unsigned long lstSonicCheck;
  
  int getSonicDistanceold();
	//IR obstacle Sensor
	int obstacleL;  
	int obstacleR;  
	//IR BlackLine detector
	int BlackLine1;  
	int BlackLine2;  
	int BlackLine3; 
	int BlackLine4;  
	int BlackLine5;  
	int FrontSensor;
	
};




#endif