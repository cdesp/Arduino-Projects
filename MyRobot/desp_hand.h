#ifndef Hand_h
#define Hand_h
#include "Arduino.h" 
#include <Servo.h>

const int fingOpen=0;
const int fingClose=180;
const int fO=0;
const int fC=180;



class Finger {
  Servo *servo;
  int maxpos;
  int minpos;
  int curpos;
  int vcurpos;
  bool reversed; //if true then max=min and min =max 
public:  
  Finger(int mnpos,int mxpos,bool rev=false);
  void Setup(int pin);
  void gotoPos(int newvpos, bool nodelay=false);
  int getVpos();
private:
  void gotoRealpos(int newpos, bool nodelay=false);
  int translatePos(int vPos);
};

class Hand {
    Finger *fingers[5];
    Servo *wrist;
    int wrpos;
    int nvp[5];
    int incr;
public:
  Hand(int Speed=4);
  void Setup(int pn1, int pn2, int pn3,int pn4, int pn5, int pnwr);
  void moveFinger(int fno, int newpos);
  void movAllfingers();
  void setPosfingers(int npf1, int npf2, int npf3, int npf4, int npf5);
  void moveWrist(int newpos);
private:
  void createFingers();
  void movFinger(int fno);
};




#endif
