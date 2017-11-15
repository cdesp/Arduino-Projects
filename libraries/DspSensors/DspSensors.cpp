#include "DspSensors.h"

extern int trPin;
extern int ecPin;

//IRrecv irrecv(RECV_PIN); //Irreceiver ;
//decode_results results; //ir results
NewPing sonar(trPin, ecPin,400); // NewPing setup of pins and maximum distance.


DESP_Sensors::DESP_Sensors() {
  blLine=blNone;
  lstCheck=0;
  lstSonicCheck=0;
}

void DESP_Sensors::init() {
	//IR Receiver
//	irrecv.enableIRIn(); // Start the receiver
  
  
   //sonic sensor
 // pinMode(trigPin, OUTPUT);
 // pinMode(echoPin, INPUT);
  
  
   //iR obstacle Sensor
//  pinMode(obstacleL, INPUT);
//  pinMode(obstacleR, INPUT);   
//  dsppullUp(obstacleL);
//  dsppullUp(obstacleR);
    
 //Line detector   
  pinMode(BlackLine1, INPUT_PULLUP);
  pinMode(BlackLine2, INPUT_PULLUP);  
  pinMode(BlackLine3, INPUT_PULLUP);
  pinMode(BlackLine4, INPUT_PULLUP);  
  pinMode(BlackLine5, INPUT_PULLUP);
  pinMode(FrontSensor, INPUT_PULLUP);



}




int DESP_Sensors::getIRbutton(){
  
if ((millis()-lstCheck>1000) ){//&&  irrecv.decode(&results)) {

 //Serial.println(results.value, HEX);

  // Receive the next value
 lstCheck=millis();
// irrecv.resume();

 return 0;//results.value;
}  
else return 0;
}

char DESP_Sensors::getIRcommand(){
 int b;  
 char c;
 
 b=getIRbutton();
 switch (b) {
   
  case  0xFF629D:  //UP BUTTON
     c= 'w';
     break;
  case  0xFFA857:  //down BUTTON
     c= 's';
    break;
  case  0xFF22DD:  //left BUTTON
     c= 'a';
    break;
  case  0xFFC23D:  //right BUTTON
     c= 'd';
    break;
  case  0xFF02FD:  //ok BUTTON
     c= 'm';
    break;  
  case  0xFF6897:  //1 BUTTON
     c= '1';
    break;      
  case  0xFF9867:  //2 BUTTON
     c= '2';
    break;      
  case  0xFFB04F:  //3 BUTTON
     c= '3';
    break;      
  case  0xFF630CF:  //4 BUTTON
     c= '4';
    break;      
  case  0xFF18E7:  //5 BUTTON
     c= '5';
    break;      
  case  0xFF7A85:  //6 BUTTON
     c= '6';
    break;      
  case  0xFF10EF:  //7 BUTTON
     c= '7';
    break;      
  case  0xFF38C7:  //8 BUTTON
     c= '8';
    break;      
  case  0xFF5AA5:  //9 BUTTON
     c= '9';
    break;      
  case  0xFF4AB5:  //0 BUTTON
     c= '0';
    break;      
  case  0xFF42BD:  //* BUTTON
     c= '*';
    break;      
  case  0xFF52AD:  //# BUTTON
     c= '#';
    break;      


    
   default: c='~';
   break; 
 }
 
 return c;
}

int DESP_Sensors::getSonicDistanceold(){
  
long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  
  lstSonicCheck=millis();
  return  (duration/2) / 29.1;  
}


int DESP_Sensors::getSonicDistance(){
	 int echoTime=sonar.ping_median(5); //was 5
	 lstSonicCheck=millis();
	 int pingcm = NewPingConvert(echoTime, US_ROUNDTRIP_CM);
	 if (pingcm==0) pingcm=99;
	 lstSonicCheck=millis();
	 return pingcm;
}

boolean DESP_Sensors::frontIsClear(){
  
 return distanceLast>distanceMin; 
}

int DESP_Sensors::distanceFront(){
	 return distanceLast;
}

void DESP_Sensors::checkStatus(){
	if (millis()-lstSonicCheck>sonicEvery)
	  distanceLast=getSonicDistance();
	  
	checkBlackLine();
}

void DESP_Sensors::setSonicParams(int dmin,int sEvery,int trpin,int ecpin){
	distanceMin=dmin;
	sonicEvery=sEvery;
	trigPin=trpin;
	echoPin=ecpin;
}

void DESP_Sensors::setIRObstacleParams(int obstL,int obstR){
	obstacleL=obstL;
	obstacleR=obstR;
	
}

void DESP_Sensors::setBlacklineParams(int bl1,int bl2,int bl3,int bl4,int bl5,int frsens){
	BlackLine1=bl1;
	BlackLine2=bl2;
	BlackLine3=bl3;
	BlackLine4=bl4;
	BlackLine5=bl5;
	FrontSensor=frsens;
}
 
 
boolean DESP_Sensors::obstacleLeft(){
	
 //check Left obstacle
   return (digitalRead(obstacleL)==LOW);
}

boolean DESP_Sensors::obstacleRight(){
	 
	   //check Left obstacle
   return (digitalRead(obstacleR)==LOW);
}
 
void DESP_Sensors::checkBlackLine(){
  boolean ss1,ss2,ss3,ss4,ss5;
  
  
  ss1=digitalRead(BlackLine1)==LOW; //ok
  ss2=digitalRead(BlackLine2)==LOW; //ok
  ss3=digitalRead(BlackLine3)==LOW; //ok
  ss4=digitalRead(BlackLine4)==LOW; //ok
  ss5=digitalRead(BlackLine5)==LOW; //ok
  
 // BTSerial.println("BLACKLINES"); 
 // BTSerial.println(ss1); 
 // BTSerial.println(ss2); 
 // BTSerial.println(ss3); 
 // BTSerial.println(ss4);   
 // BTSerial.println(ss5);   
  
  blLine= blNone;

  if (ss5) blLine=blFarRight;
  if (ss1) blLine=blFarLeft;
  if ((ss4 and !ss3 and !ss2) or (ss4 and ss3 and !ss2) or (ss4 and !ss3 and ss2))
    blLine= blRight;
  if ((!ss4 and !ss3 and ss2) or (!ss4 and ss3 and ss2))
    blLine= blLeft;
  if ((!ss4 and ss3 and !ss2) or (ss4 and ss3 and ss2))
    blLine= blFront;
  if (ss1 and ss2 and ss3 and ss4 and ss5)
    blLine= blNone;
}

boolean DESP_Sensors::frontBumperHit(){
  
 return (digitalRead(FrontSensor)!=LOW); 
}

boolean DESP_Sensors::BLfarleft(){
  
 return (digitalRead(BlackLine1)!=LOW); 
}

boolean DESP_Sensors::BLleft(){
  
 return (digitalRead(BlackLine2)!=LOW); 
}
boolean DESP_Sensors::BLmiddle(){
  
 return (digitalRead(BlackLine3)!=LOW); 
}
boolean DESP_Sensors::BLright(){
  
 return (digitalRead(BlackLine4)!=LOW); 
}

boolean DESP_Sensors::BLfarright(){
  
 return (digitalRead(BlackLine5)!=LOW); 
}
