
#include "DspRobot.h";





DESP_Robot::DESP_Robot() {
  
  ENPin=-1;
  robomode=rc2;
  rcmd=rcStop;
  stepMoveTime=1000; //for 1 step forwrd or backward in millis
  stepTurnTime=6;//for 1 degree turn in millis
  stepMoveinCm=15;//15 cm each step
  clearDistance();
}

void DESP_Robot::init(int enp,int rmot1p1,int rmot1p2,int rmot2p1,int rmot2p2,int rmot3p1,int rmot3p2,int rmot4p1,int rmot4p2) {
	ENPin=enp;
	motor1.init(1,rmot1p1,rmot1p2);  //back
  motor2.init(2,rmot2p1,rmot2p2);//back
  motor3.init(3,rmot3p1,rmot3p2);//front
  motor4.init(4,rmot4p1,rmot4p2);  //front
  
  if (ENPin!=-1) {pinMode(ENPin, OUTPUT);digitalWrite(ENPin,LOW);}
}

void DESP_Robot::Backward(){
 Stop(false);
 if (lastrcmd!=rcBackward){
 	 delay(100);
 	 Stop(true);
 	 delay(500);
 }
 
 motor1.run(BACKWARD);
 motor2.run(BACKWARD);
 motor3.run(BACKWARD);
 motor4.run(BACKWARD);

 if (ENPin!=-1) digitalWrite(ENPin,HIGH); 
 rcmd=rcBackward;
 lastrcmd=rcmd;
}

void DESP_Robot::Forward(){
 Stop(false);
if (lastrcmd!=rcForward){
 	 delay(100);
 	 Stop(true);
 	 delay(500);
 }
 
 motor1.run(FORWARD);
 motor2.run(FORWARD);
 motor3.run(FORWARD);
 motor4.run(FORWARD);
 if (ENPin!=-1) digitalWrite(ENPin,HIGH);
 rcmd=rcForward; 
 lastrcmd=rcmd;
}

void DESP_Robot::Stop(boolean full){
 
 if (ENPin!=-1) digitalWrite(ENPin,LOW);
 if (full)
 	{ 		
 motor1.run(RELEASE);
 motor2.run(RELEASE);
 motor3.run(RELEASE);
 motor4.run(RELEASE);
 delay(25);}
 
 distanceTravelled=mydist-lastdist;
 lastdist=mydist;
 
 rcmd=rcStop;
//lastrcmd=rcmd;
}

void DESP_Robot::left(){
 Stop(true);
if (lastrcmd!=rcLeft) delay(500);

 
 motor1.run(FORWARD);
 motor2.run(BACKWARD);
 motor3.run(FORWARD);
 motor4.run(BACKWARD);
 if (ENPin!=-1) digitalWrite(ENPin,HIGH);
  rcmd=rcLeft;
  lastrcmd=rcmd;
}

void DESP_Robot::right(){
 Stop(true);
 if (lastrcmd!=rcRight){ delay(500); }
 	
 motor1.run(BACKWARD);
 motor2.run(FORWARD);
 motor3.run(BACKWARD);
 motor4.run(FORWARD);
 if (ENPin!=-1) digitalWrite(ENPin,HIGH);
  rcmd=rcRight;
  lastrcmd=rcmd;
}

String DESP_Robot::getCurrentCommand(){


 	switch (rcmd){
 		 case rcBackward: 
 		 	return "BACK";
 		 	break;
  	 case rcForward:
      return "FORW";
			break;  	 	
  	 case rcStop:
  	 	return "STOP"; 
			break;  	 	
  	 case rcLeft:
  	 	return "LEFT"; 
  		break;
  	 case rcRight:
  	 	return "RIGT"; 
 			break;
 		
 	}
	
}


void DESP_Robot::stepForward(){
	clearDistance();
  Forward();
  motortime=millis();
}

void DESP_Robot::stepBackward(){
  clearDistance();
  Backward();
  motortime=millis();
}

void DESP_Robot::turnLeft(int deg){  
  lastAngle=deg;
  left();
  motortime=millis();
}

void DESP_Robot::turnRight(int deg){  
  lastAngle=deg;  
  right();
  motortime=millis();
}

String DESP_Robot::checkTimetoStop(){

	if (rcmd==rcStop) return "";
	else	
	if ( roboTimeIsUp()){		
		Stop(false);
		return "Stopped: time is up!!!";
	}
	else
		 return "";
	
}

boolean DESP_Robot::isTimeUp(unsigned long v,int maxtime){
	return (millis()-v)>maxtime;
}

boolean DESP_Robot::moveIsUp(){
	return (rcmd==rcForward || rcmd==rcBackward) && isTimeUp(motortime,stepMoveTime);
}

boolean DESP_Robot::turnIsUp(){
	return (rcmd==rcLeft || rcmd==rcRight) && isTimeUp(motortime,lastAngle*stepTurnTime);
}

boolean DESP_Robot::moveIsUpCM(){
	return (rcmd==rcForward || rcmd==rcBackward) && isDistanceReached();
}


boolean DESP_Robot::isDistanceReached(){	
	return (abs(distanceNowCM())>=stepMoveinCm);
}

boolean DESP_Robot::roboTimeIsUp(){
  if (robomode!=rc){
    if (useTimeDistance)
     return moveIsUp() || turnIsUp();
    else
  	 return moveIsUpCM() || turnIsUp();
  }
  else
  	 return false; 
}

void DESP_Robot::setNextMode(){
   switch (robomode) {
   case progload:
           robomode=rc;
           break;
   case rc:
           robomode=rc2;
           break;
   case rc2:
           robomode=robot;
           break;
   case robot:
           robomode=robotbl;
           break;
   case robotbl:
           robomode=progload;
           break;
  }
  
}

RoboCommand DESP_Robot::RoboState(){
	return rcmd;
	
}

int DESP_Robot::distanceNowCM(){
	return inCM(mydist-lastdist);
}

void DESP_Robot::clearDistance(){
	  mydist=0;
    distanceTravelled=0;
}

float DESP_Robot::inCM(int dist){

  return mydist/0.85;
}

void DESP_Robot::motorFrontLeft(boolean FORW){

 if (FORW)
 	 motor4.run(FORWARD);
 else	
   motor4.run(BACKWARD);
if (ENPin!=-1) digitalWrite(ENPin,HIGH);	   
}
	
void DESP_Robot::motorFrontRight(boolean FORW){

 if (FORW)
 	 motor3.run(FORWARD);
 else	
   motor3.run(BACKWARD);
if (ENPin!=-1) digitalWrite(ENPin,HIGH);		
}

void DESP_Robot::motorBackLeft(boolean FORW){

 if (FORW)
 	 motor2.run(FORWARD);
 else	
   motor2.run(BACKWARD);
if (ENPin!=-1) digitalWrite(ENPin,HIGH);		
}
void DESP_Robot::motorBackRight(boolean FORW){
	
 if (FORW)
 	 motor1.run(FORWARD);
 else	
   motor1.run(BACKWARD);
if (ENPin!=-1) digitalWrite(ENPin,HIGH);	
}


