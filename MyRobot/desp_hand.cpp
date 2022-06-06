
#include "desp_hand.h"


//class finger 
//we will have a virtual position 0-180  0 means open 180 close
//we will translate this to maxpos- minpos

Finger::Finger(int mnpos,int mxpos,bool rev){
  servo = new Servo;
  maxpos=mxpos;
  minpos=mnpos;
  reversed=rev;
  curpos= rev==false?minpos:maxpos;  //select minpos
  vcurpos= rev==false?0:180; 
}

void Finger::Setup(int pin){
  servo->attach(pin);
  servo->write(curpos); //goto at start position 
}

void Finger::gotoRealpos(int newpos, bool nodelay){

    if (newpos==curpos) return;
    int add= newpos>curpos?1:-1;
     
   // Serial.print(curpos);Serial.print(" "); Serial.print(newpos);Serial.print(" "); Serial.print(add);Serial.print(" ");
    for (int pos = curpos;pos != newpos ; pos += add) { // goes from 0 degrees to 180 degrees in steps of 1 degree
      // Serial.print(pos);Serial.print(" ");
       if (pos>minpos && pos<maxpos){
         //Serial.print(pos);Serial.print(" ");
         servo->write(pos);              // tell servo to go to position in variable 'pos'
         if (!nodelay)
          delay(15);                       // waits 15 ms for the servo to reach the position
          
       }       
  }
  curpos=newpos;
  servo->write(curpos);
//Serial.println("");
}

//translate 0-180 to minpos-maxpos or maxpos-minpos
int Finger::translatePos(int vPos){
  
  int nvpos= reversed==false?vPos:180-vPos;
  int rpos= trunc(((nvpos*(maxpos-minpos))/180))+minpos; //translate virtual to real
  //Serial.print(minpos);Serial.print(",");Serial.print(maxpos);Serial.print(" ");Serial.println(rpos);
  rpos = rpos<minpos?minpos:rpos;
  rpos = rpos>maxpos?maxpos:rpos;
  vcurpos=nvpos;
  return rpos; 
}

//new virtual pos 0 -180
void Finger::gotoPos(int newvpos, bool nodelay){
  int tt=translatePos(newvpos);
  //Serial.print(newvpos);Serial.print("-->");Serial.println(tt);
  gotoRealpos(tt, nodelay);
}

int Finger::getVpos(){
  return reversed==false?vcurpos:180-vcurpos;
}

//-------------------

Hand::Hand(int Speed){
  incr=Speed;
  wrpos=90;
}

//pn1 is the little finger
void Hand::Setup(int pn1, int pn2, int pn3,int pn4, int pn5, int pnwr){
  createFingers();
  wrist = new Servo;
  fingers[0]->Setup(pn1);
  fingers[1]->Setup(pn2);
  fingers[2]->Setup(pn3);
  fingers[3]->Setup(pn4);
  fingers[4]->Setup(pn5);
  wrist->attach(pnwr);
}

void Hand::createFingers(){
  fingers[0]= new Finger(70,180,true); //little
  fingers[1]= new Finger(0,120); 
  fingers[2]= new Finger(0,120); 
  fingers[3]= new Finger(0,120); 
  fingers[4]= new Finger(90,180,true);  //thumb  
  for (int i=0;i<5;i++){
    nvp[i]=-1;
  }
}

//pos is the newposition 0-180 0 opened 180 closed
void Hand::moveFinger(int fno, int newpos){
     fingers[fno]->gotoPos(newpos);  
     nvp[fno]=-1;  //disable automove 
}

void Hand::setPosfingers(int npf1, int npf2, int npf3, int npf4, int npf5){
  nvp[0] = npf1; nvp[1] = npf2; nvp[2] = npf3; nvp[3] = npf4; nvp[4] = npf5;
}

void Hand::movFinger(int fno){
  int cvp=fingers[fno]->getVpos();
  int np=nvp[fno];

  if (cvp==np || np==-1) return;
//Serial.print(cvp);Serial.print(" ");Serial.println(np);  
  int add= (np-cvp>0)?incr:0-incr;
  if (abs(np-(cvp+add))<=incr)
   fingers[fno]->gotoPos(np, true); 
  else
   fingers[fno]->gotoPos(cvp+add, true); 
  
}

//moves 1 degree all fingers until reaching the required pos
void Hand::movAllfingers(){
  for (int i=0;i<5;i++){
    movFinger(i);
  delay(15);  
  }
}

void Hand::moveWrist(int newpos){
  wrist->write(newpos);
  delay(15); 
  wrpos=newpos;
}
