#include "CPSonicSensor.h"



CP_SonicSensor::CP_SonicSensor() {
  lstSonicCheck=0;
  distanceMin=25;
  sonicEvery=100;
  trigPin=0;
  echoPin=0;  
}

void CP_SonicSensor::init() {
   //sonic sensor
  if (trigPin==0) return;
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  sonar = new NewPing(trigPin, echoPin,400); // NewPing setup of pins and maximum distance.
  //NewPing sonar(trigPin, echoPin,400); // NewPing setup of pins and maximum distance.
}  



int CP_SonicSensor::getSonicDistance(){
   if (sonar==NULL) return -1;
   int echoTime=sonar->ping_median(5); //was 5
   lstSonicCheck=millis();
   int pingcm = NewPingConvert(echoTime, US_ROUNDTRIP_CM);
   if (pingcm==0) pingcm=99;
   lstSonicCheck=millis();
   return pingcm;
}

boolean CP_SonicSensor::frontIsClear(){  
 return distanceLast>distanceMin; 
}

int CP_SonicSensor::distanceFront(){
   return distanceLast;
}

void CP_SonicSensor::checkStatus(){
  if (millis()-lstSonicCheck>sonicEvery)
    distanceLast=getSonicDistance();    
}

void CP_SonicSensor::setSonicParams(int dmin,int sEvery,int trpin,int ecpin){
  distanceMin=dmin;
  sonicEvery=sEvery;
  trigPin=trpin;
  echoPin=ecpin;
}


