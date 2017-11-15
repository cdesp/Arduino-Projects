
#ifndef CPSonicSensor_H
 #define CPSonicSensor_H

 #include <NewPing.h>



class CP_SonicSensor
{
 public:
  
  CP_SonicSensor();
  void init();
    
  void setSonicParams(int dmin,int sEvery,int trpin,int ecpin);
  int getSonicDistance();
  int distanceFront();
  boolean frontIsClear();
  void checkStatus();
 private:
  
  //sonic sensor
  NewPing* sonar;
  int trigPin,echoPin;
  int distanceMin,distanceLast,sonicEvery;
  unsigned long lstSonicCheck;
   
};




#endif
