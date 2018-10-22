#include "DspGyro2.h"


 


DESP_Gyro::DESP_Gyro(int addr) {
  
  qmc= new MechaQMC5883 ;
  
  redogyro=0;
  turning=false;
  targbearing=0;
  azimuth=0;
}



void DESP_Gyro::checkGyro(boolean debug){
  int x, y, z;
  
  qmc->read(&x, &y, &z,&azimuth);  
  
}

void DESP_Gyro::calibrateGyro(boolean debug){
  //Not Needed
}
 
 
void DESP_Gyro::init(){
  Wire.begin();
  qmc->init();
  qmc->setMode(Mode_Continuous,ODR_200Hz,RNG_2G,OSR_256);

}

void DESP_Gyro::goLeft(int deg){
   checkGyro(true);
   curbearing=azimuth;
   targbearing=curbearing-deg;
   if (targbearing<-180)
    targbearing=360+targbearing;
   turning=true;
//   Serial.print("Turning 90 degrees left from ");
//   Serial.print(curbearing,DEC);
//   Serial.print(" to ");
//   Serial.println(targbearing,DEC);       
}

void DESP_Gyro::goRight(int deg){
   checkGyro(true);
   curbearing=azimuth;
   targbearing=curbearing+deg;
   if (targbearing>180)
    targbearing=targbearing-360;
   turning=true;
  // Serial.print("Turning 90 degrees right from ");
  // Serial.print(curbearing,DEC);
  // Serial.print(" to ");
  // Serial.println(targbearing,DEC);       
}

int DESP_Gyro::getBearingDistance(float b1,float b2){
  int b1b;
  int b2b;
  int d3,g3,i3,j3,k3;
 
 
 
  //convert to 360
  if (b1<0) b1b=180+180+b1; else b1b=b1;
  if (b2<0) b2b=180+180+b2; else b2b=b2;  
 
  d3=b1b-b2b;
  g3=d3-360;
  i3=int(g3/360);
  if(abs(d3)<180) j3=d3;
    else j3=g3-i3*720;
 
  return j3;     // j3<0 then go right else go left
}

boolean DESP_Gyro::isTargetReached(int test){
  
	return abs(getBearingDistance(targbearing,curbearing))<=test;
}

void DESP_Gyro::setCurrentPosition(){
	curbearing=azimuth;
}

int DESP_Gyro::getDistance(){
	return getBearingDistance(curbearing,targbearing);
}

void DESP_Gyro::targetReached(){
	turning=false;
}

void DESP_Gyro::getGyroSettings(boolean debug){
	redogyro=0;
  //checkGyro(debug);
  //RemDebug("GGS");
  do {  	   
       if (redogyro++<6){
         delay(5);//was 10
         checkGyro(debug);
         //checkGyro(true);
        
       } else break;               
  } while  ((azimuth==180) or (azimuth==0)); //sometimes is the wrong output so we recheck to be sure
  
  setCurrentPosition();
}

void DESP_Gyro::gotoBearing(float b){
	targbearing=b;
	turning=true;	
}

float DESP_Gyro::getSafeGyroPos(){
   int safecnt=0;
   int sum;
   
   // RemDebug("SGP");
    getGyroSettings(false);            
    
    do{    	
     sum+=curbearing;
     getGyroSettings(false);          
    } while (safecnt++<10  );//for safety we take measurements
    curbearing=sum / 10;
 return curbearing;  
}
