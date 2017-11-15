#include <Arduino.h>
#include <NewPing.h>
//#include <DspExpander.h>
#include <DspMotor.h>
#include <DspRobot.h>
#include <DspSensors.h>
                                           
#include <DspStepper.h>
#include <DspInterpreter.h>
                           
#include <helper_3dmath.h>
#include "MPU6050_6Axis_MotionApps20.h"
#include <DspGyro.h>
#include <I2Cdev.h>
#include <Wire.h>                                     
//#include <PCF8575.h>                        
#include "SoftwareSerial.h"                            
#include <LiquidCrystal.h>              
#include <Speaker.h>
#include <Servo.h>
#include "NewCar_mega.h"

#define HALFSTEP 8

                      
//PCF8575 expander;
boolean useexpander=false;

                        
SoftwareSerial BTSerial(12, 13);                                       
LiquidCrystal lcd(6, 7, 8, 9, 10, 11);                   



                     
DESP_Gyro dspgyro(MPU6050_DEFAULT_ADDRESS);
DESP_Robot Myrobo;
DESP_Sensors Mysensors;
DESP_Interpreter interpreter;
                                   
                                                                                        
                                                                                   
Servo MyServo;                 

int speakerPin=A0;
Speaker speaker(speakerPin);


char lastcmd=' ';
char nextcmd=' ';


                    
                                                                   
void speedencint()
{
   if (Myrobo.RoboState()==rcForward)  Myrobo.mydist++;
   if (Myrobo.RoboState()==rcBackward)  Myrobo.mydist--;
}



void DoDebug(String s,int n){
 Serial.print(s);
 Serial.println(n,DEC);
}


unsigned long tm9=0;
void myremdebug(String s){
  if (millis()-tm9>200) {
   BTSerial.println(s);  
   tm9=millis();
  }
}

void mybeep(){
 speaker.PlayTone(500,110);

}

void setup() {
  useexpander=false;


  Serial.begin(115200);                                                 
  Serial.println("Serial comm ok!");                                 
  
  interpreter.evaluate=evalfunc;
  
                      
  BTSerial.begin(9600);
  BTSerial.flush();
  BTSerial.println("Bluetooth On ...");

       
                                                  
  lcd.begin(16, 2);
                                
  lcd.print("LCD OK....");
  delay(300);

  lcd.clear();
  lcd.print("Gyro Calibrating ...");
               

  dspgyro.RemDebug=myremdebug;
  dspgyro.init();


  Mysensors.setSonicParams(25,100,trPin,ecPin);                        
  Mysensors.setIRObstacleParams(obstacleL,obstacleR);
  Mysensors.setBlacklineParams(BlackLine1,BlackLine2,BlackLine3,BlackLine4,BlackLine5,frontSensor);
  Mysensors.init();


  lcd.clear();
  lcd.print("Sensors OK....");
  delay(500);

                
  Myrobo.init(motEN,mot1p1,mot1p2,mot2p1,mot2p2,mot3p1,mot3p2,mot4p1,mot4p2);
  Myrobo.stepMoveTime=200;                  
  Myrobo.stepTurnTime=6;                    
  Myrobo.stepMoveinCm=10;                     
  Myrobo.useTimeDistance=false;                                           
  Myrobo.Stop(true);
  pinMode (speedenc, INPUT) ;
  attachInterrupt(speedint, speedencint, CHANGE);
  lcd.clear();
  lcd.print("Robo OK....");
               

  MyServo.attach(A1);                                                                                                                            
  DoTurn(511);
  delay(500);
  DoTurn(-1023);
  delay(500);
  DoTurn(511);   

  mybeep();delay(500);mybeep();delay(100);mybeep();delay(500);mybeep();
  Serial.println("Setup ok!");    

  pinMode(3, OUTPUT);           // set pin 3 to output (laser Pin)
  digitalWrite(3, LOW);       // turn off
                         
  lcd.print("Setup OK ...");
}

int lastpos=0;
void DoTurn(int pos){
  int v;
                                                                 
                              
 
                                     
  pos=lastpos+pos;
  lastpos=pos;
                        
  v=map(pos,-1024,1024,0,180);
  MyServo.write(v);
  delay(15);
                          
}

                         
void RCloop(){

  checkobstacle();

  switch (lastcmd){
  case 'w':
    if (Mysensors.frontIsClear())
      Myrobo.Forward();              
    break;
  case 's':
    Myrobo.Backward();
    break;
  case ' ':  
  case '*':
    Myrobo.Stop(true);
    break;
  case 'a':
    Myrobo.left();
    break;
  case 'd':
    Myrobo.right();
    break;       
     case 'r':                  
              Myrobo.motorFrontLeft(true);
              break;
     case 'f':                 
              Myrobo.motorBackLeft(true);
              break;
     case 'y':                   
              Myrobo.motorFrontRight(true);
              break;
     case 'h':                  
              Myrobo.motorBackRight(true);
              break;   
    
  }  
}


               
String mes1;

void RCloop2(){

  checkobstacle();

  mes1=Myrobo.checkTimetoStop();                             
  if (mes1!="") {
    Serial.println("MSG:"+mes1);   
  }


  switch (lastcmd){
  case 'w':
    if (Mysensors.frontIsClear())
      Myrobo.stepForward();              
    break;
  case 's':
    Myrobo.stepBackward();
    break;
  case ' ':
  case '*':    
    Myrobo.Stop(true);
    break;
  case 'a':
    Myrobo.turnLeft(90);
    break;
  case 'd':
    Myrobo.turnRight(90);
    break;


  }


}


                     
String Err="";

void Robotloop(){

                                                  
                                    
  checkobstacle();

  if (Myrobo.RoboState()==rcStop) {                       
                                   
    if ((Myrobo.robomode==robotbl) && (Mysensors.blLine !=blNone)){
      switch (Mysensors.blLine){
      case blLeft: 
        Myrobo.turnLeft(15);
        break;
      case blFarLeft: 
        Myrobo.turnLeft(35);
        break;
      case blRight: 
        Myrobo.turnRight(15);
        break;
      case blFarRight: 
        Myrobo.turnRight(35);
        break;            
      case blFront: 
        if (Mysensors.frontIsClear()){
          Err="";
          Myrobo.stepForward();
        }
        break;

      } 

    }
    else
    {
                                          
      if (Mysensors.frontIsClear()){
        Err="";
        Myrobo.stepForward();
      }
      else
      {
                                                       
        Myrobo.Stop(true);
        if (!obstleft()) Myrobo.turnLeft(20);
        else
          if (!obstright()) Myrobo.turnRight(20);
          else {
            Err="No way Out. Halted.";  
            Myrobo.stepBackward();          
          }
                                                             
      }              

    }             
  }          
  Mysensors.checkStatus();
                                             
                           
}


int readSonicSensor(){
  int d1,d2,d3;

  delay(200);  
  d1=Mysensors.getSonicDistance();
  delay(200);
  d2=Mysensors.getSonicDistance();
  delay(200);
  d3=Mysensors.getSonicDistance(); 
  return min(min(d1,d2),d3);
  
}

int distSensorFront(){
  int dout;

  
  dout=readSonicSensor();
  if (dout==99) {
    delay(250);
    dout=distSensorFront();    
  }
  BTSerial.print("Dist Front:");
  BTSerial.println(dout);
  return dout;
}


int distSensorLeft(){
  int dout;

  
  DoTurn(1023);
  dout=readSonicSensor();
  if (dout==99) {
    distSensorLeftReset();
    delay(250);
    dout=distSensorLeft();    
  }
  BTSerial.print("Dist Left:");
  BTSerial.println(dout);
  return dout;
}

void distSensorLeftReset(){
  DoTurn(-1023);
  
}


int distSensorRight(){
  int d1,d2,d3,dout;
    
  DoTurn(-1023);
  dout=readSonicSensor();
  if (dout==99) {
    distSensorRightReset();
    delay(250);
    dout=distSensorRight();    
  }
  BTSerial.print("Dist Right:");
  BTSerial.println(dout);  
  return dout;
}

void distSensorRightReset(){
  DoTurn(1023);
  
}

boolean obstleft(){
  boolean ret;

  distSensorLeft();
  showinfo1(); 
  BTSerial.println(Mysensors.distanceFront());
  ret= !Mysensors.frontIsClear(); 
  distSensorLeftReset();
  return ret;
}

boolean obstright(){
  boolean ret;

  distSensorRight();
  showinfo1(); 
  BTSerial.println(Mysensors.distanceFront());
  ret= !Mysensors.frontIsClear(); 
  distSensorRightReset();
  return ret;
}

void checkobstacle(){
  
  Mysensors.checkStatus();

  if (!Mysensors.frontIsClear() && (Myrobo.RoboState()==rcForward)) {
    Myrobo.Stop(true);

    Serial.println("Stopped not clear ahead!!!");  
  } 
                                    

                        
                            
      
}

              
void showinfo1(){                       
                                 
  lcd.setCursor(0, 1);

  lcd.print("d");
  lcd.print(Mysensors.distanceFront());
  lcd.print(" ");                           
                                             
}

String smode;

void showinfo0(){
                        
                                   
  lcd.setCursor(0, 0);
  if (Err!="") 
    lcd.print(Err);     
  else { 
    lcd.print(smode);
    lcd.print("   ");
    lcd.setCursor(6, 0); 
                       
                                                 
    lcd.print(Myrobo.getCurrentCommand());
    lcd.print("   ");
    lcd.setCursor(12, 0);    
    lcd.print(lastcmd);
    lcd.print("   ");
    lcd.setCursor(14, 0);  
    if (Mysensors.blLine!=blNone)        
      lcd.print("BLN");
     else lcd.print("   ");

  }
}

void showBLinfo(){
  if (Myrobo.robomode==robotbl)
    switch (Mysensors.blLine){

    case blNone:
                                         
      break;
    case blLeft:
      BTSerial.println("black Left");
      break;
    case blRight:
      BTSerial.println("black Right");
      break;
    case blFarLeft:
      BTSerial.println("black Far Left");
      break;
    case blFarRight:
      BTSerial.println("black Far Right");
      break;    
    case blFront:
      BTSerial.println("Black Front");
      break;


    } 

}


            


unsigned long tm2=0;
unsigned long tm3=0;

void loop3()
{



                         
                                             
                                        
  if (Serial.available()>0)
  {
    int steps = Serial.parseInt();
    DoTurn(steps);
    Serial.println("ok");
  }  

}


                           
boolean inprogram=true;




int converttoint(byte b1,byte b2){
  return b2*256+b1;  
 
}

int ChkByte=0;

int getNumberFromBT(){
 byte b1,b2;

 while (!BTSerial.available()){ 
  if (BTSerial.overflow()) {
   Serial.println("SoftwareSerial overflow!"); 
  }
 }
 
 b1=BTSerial.read();           
 while (!BTSerial.available()) {
  if (BTSerial.overflow()) {
   Serial.println("SoftwareSerial overflow!"); 
  }
 }
 b2=BTSerial.read();         
 if (b1==253)
   return b1;
 else
 if (b2==253)
   return b2;   
 if (b1!=-1 || b2!=-1) {
   ChkByte = ChkByte ^ b1;
   ChkByte = ChkByte ^ b2;
   return converttoint(b1,b2);
 }
 else
   return -1;  
  
}

boolean getProgramFromBT()
{
  int siz,t;
  int i=0;
  int c,p1,p2;


  DoDebug("Get new program",0);
  lcd.setCursor(0, 0);  
  lcd.print("In Programming..          ");  
  t= millis();
  do{
     BTSerial.write('R'); 
     delay(50);
  }
  while ((BTSerial.available()==0) and (millis()-t<5000));
  
  siz=BTSerial.read();
  DoDebug("Prog size:",siz);
  
  if (siz<1) {
   Serial.println("Invalid Size. abort");
    Myrobo.robomode=rc2;                   
    return false;
  }

  Serial.println("Read Program");
  delay(400);
  interpreter.newProgram();  
  ChkByte=0;
  do
  {
    c=getNumberFromBT();
    if (c!=253){
      p1=getNumberFromBT();
      p2=getNumberFromBT();    
    }
       Serial.print("Inp Byte: ");
       Serial.print(c,DEC);    
       Serial.print(" ");     
       Serial.print(p1,DEC);  
       Serial.print(" ");            
       Serial.println(p2,DEC);        
    if ((c!=253) && (c!=-1)) {
      interpreter.addCommand(c,p1,p2);          
    }        
  } while (c!=253);                        
  BTSerial.write('R'); 
  do {
    c=BTSerial.read();
  } while ((c==253) or (c==-1));
  
  if (ChkByte==253) ChkByte=254;
  
  int testchkbyte=c;  
  if (testchkbyte==ChkByte){  
    Serial.println("Program Read ok");
    BTSerial.println("Program Transfer ok");
  }
  else   {
    Serial.println("CRC Error!!!");
    Serial.println(testchkbyte,DEC);
    Serial.println(ChkByte,DEC);
    lcd.setCursor(0, 1);
    lcd.print("CHECKSUM ERROR");
    mybeep();delay(100);mybeep();delay(100);mybeep();delay(100);mybeep();
    delay(5000);
    return false;
  }
  delay(1000);
  while (BTSerial.available()){                    
    BTSerial.read();    
  }
  return true;
     
}

                                  
boolean evalfunc(DspCommand *cmd){
  
  boolean retval=true;  
  int checkparam;
  
  DoDebug("testing:",cmd->cmdid);
  checkparam=cmd->param2;
  switch(cmd->cmdid){
     case 70:                                       
             DoDebug("LOOP COMMAND",0);
             return true;
             break;
     case 80:                      
          //Mysensors.checkStatus();
          retval=distSensorFront()>checkparam;          
          break;     
     case 81:                   
          //Mysensors.checkStatus();
          retval=distSensorFront()<checkparam;
          break;     
     case 82:                                      
          retval=distSensorRight()>checkparam;    
          distSensorRightReset();          
          break;     
     case 83:                         
          retval=distSensorRight()<checkparam;    
          distSensorRightReset();
          break;     
     case 84:                           
          retval=distSensorLeft()>checkparam;    
          distSensorLeftReset();
          break;     
     case 85:                        
          retval=distSensorLeft()<checkparam;    
          distSensorLeftReset();
          break;     
     case 86:  
          break;     
     case 87:  
          break;     
     case 88:                    
          retval=!Mysensors.BLleft();
          break;     
     case 89:                     
          retval=!Mysensors.BLright();
          break;     
     case 90:                        
          retval=!Mysensors.BLfarleft();
          break;     
     case 91:                         
          retval=!Mysensors.BLfarright();
          break;     
     case 92:                      
          retval=!Mysensors.BLmiddle();
          break;     
     case 93:              
          retval=Mysensors.frontBumperHit();
          break;     
     case 94:                      
          retval=digitalRead(checkparam)==HIGH;
          break;     
     case 95:                     
          retval=digitalRead(checkparam)==LOW;
          break;     
     case 96:  
          break;     
     case 97:  
          break;     
     case 98:  
          break;     
          
          
  }
  if (cmd->param1!=1)  return retval; else return !retval;
  
}

int lastcmdid=-1;
float straightbearing=-1;
boolean lastcommandexecuted=true;

boolean programExecute(){

  DspCommand *cmd;
  int com=-1;
  int d,n;
  float gp;
  int bd;
  
  if (dspgyro.turning) return false; 
  if (!interpreter.running) return false;
  
  
  if (lastcommandexecuted){
     cmd=interpreter.getCommandToExecute();     
     if (cmd!=0){
        DoDebug("Command:",cmd->cmdid); 
        com=cmd->cmdid;
        lastcommandexecuted=false;
     }  else  com=2;               
  }  
  else
     com=lastcmdid;
    
                                                                                       
  

     
   if (com>-1)
     switch(com) {
       case 1:break;
       case 2:interpreter.endProgram();       
              DoDebug("PROGRAM TERINATED!!!",0);
              DoDebug("====================",0);
              break;
       case 5:                         
               n=cmd->param1;
               if (n<5) n=5;
               if (n>100) n=100;
               Myrobo.stepMoveinCm=n;
               break;
       case 10:                  
               gp=dspgyro.curbearing;
               DoDebug("Bearing:",gp);
               if (com!=lastcmdid){
                 straightbearing=gp;
                 DoDebug("StraightBearing:",gp);
               }

                bd=dspgyro.getBearingDistance(straightbearing,gp);
                DoDebug("distance:",bd);
                if (abs(bd)<5)
                   Myrobo.stepForward();                
                else
                {                                  
                  mybeep();
                  dspgyro.gotoBearing(straightbearing);
                  DoDebug("Straight:",straightbearing);
                  lastcmdid=com;
                  return false;                       
                }
              break;   
       case 20:
               Myrobo.stepBackward();
              break;
       case 30:
               dspgyro.goRight(cmd->param1);
              break;
       case 40:
               dspgyro.goLeft(cmd->param1);               
              break;           
       case 50:
              Myrobo.Stop(true);              
       case 70:                      
              break;       
       case 80:                                              
              
              break;
       case 99:                          
              break;
       case 100:                                  
              break;
              
       case 110:
                speaker.Beep();
                break;              
       case 111:n=cmd->param1;
                d=cmd->param2;
                speaker.PlayTone(n,d);
                break;
       case 200:                      
                pinMode(cmd->param1, OUTPUT);
                break;
       case 201:                     
                pinMode(cmd->param1, INPUT);
                break;
       case 202:                    
                digitalWrite(cmd->param1, HIGH);   
                break;   
       case 203:                      
                digitalWrite(cmd->param1, LOW);
                break;                   
       case 250:
                delay(cmd->param1);
                break;                               
      }        
      
  interpreter.commandExecuted();
  if (com==10 || com==20 || com==30 || com==40)                     
    lastcmdid=com;
  lastcommandexecuted=true;
  
  return !(com==10 || com==20 || com==30 || com==40);                    
}


void DoProgram()
{ boolean f;
  unsigned long tm;
  
  if (inprogram==true){
    straightbearing=-1;
    lastcmdid=-1;
    lastcommandexecuted=true;    
    if (getProgramFromBT()){
     lcd.setCursor(0, 0);
     lcd.print("                 ");
     lcd.print("Executing...");  
     inprogram=false;
     DoDebug("Execute Program",0);
                                                                                            
     interpreter.startProgram();                                             
    }
  }
  else {
    if (Myrobo.RoboState()==rcStop){                                   
                                                
       tm=millis();
       do {
          f=programExecute();                    
          if (!interpreter.running) {
             Myrobo.robomode=rc2;                    
             DoDebug("Program Terminated!!!",0);
          } 
       } while (f and (millis()-tm<1000));                                                               
    } else
        DoDebug("RoboState:",Myrobo.RoboState());
  }     
}


int lastprintbearing;
float beardist=0;
float lastbeardist=0;


float getGyroDeviation(){
    
  
  dspgyro.getSafeGyroPos();   
   
  if (dspgyro.turning ){
                          

                              
   if (dspgyro.isTargetReached(maxdeviation)){                                   
    Myrobo.Stop(true);        
    dspgyro.getSafeGyroPos();     
    if (dspgyro.isTargetReached(maxdeviation)){                                   
      dspgyro.targetReached(); 
      DoDebug("Target reached!!!",0);
      DoDebug("current:",dspgyro.curbearing); 
      DoDebug("target:",dspgyro.targbearing);
      DoDebug("Deviation:",dspgyro.getDistance());      
      dspgyro.getGyroSettings(true);
      return 0;
    }
    else
     return dspgyro.getDistance();
   }
   else  
     return dspgyro.getDistance();   
  }
  else
   return 0;
}

unsigned long looptm=0;
unsigned long functm=0;
unsigned long tm5=0;
unsigned long tm4=0;


void loop()
{

  
  String st;
  int turndeg=0;
  
  beardist=getGyroDeviation();       

  st=Myrobo.checkTimetoStop();
  if (st!="") Serial.println(st);      

  if (!dspgyro.turning && Myrobo.robomode!=progload) checkobstacle();       

  lastcmd='~';
                                                  
  if (lastcmd=='~') lastcmd=BTSerial.read();
  if (int(lastcmd)==-1) lastcmd=Serial.read();


  if ((lastcmd!='~') && (int(lastcmd)!=-1)) {
                                
    Serial.println(int(lastcmd));

    switch (lastcmd) {
    case '0':                            
      dspgyro.targetReached(); 
      Myrobo.robomode=rc2;
      Myrobo.Stop(true);
      BTSerial.println("Program terminated!!!");
      break;            
    case '1':
      inprogram=true;
      Myrobo.robomode=progload;
      delay(500);
      break;
    case '2':
      Myrobo.robomode=rc;
      break;
    case '3':
      Myrobo.robomode=rc2;
      break;
    case '4': 
      Myrobo.robomode=robot;
      break;     
    case '5': 
      Myrobo.robomode=robotbl;
      break;                      
    case 'm':
      Myrobo.setNextMode();
      break;
    case 'o':
      DoTurn(113);            
      break;       
    case 'p':
      DoTurn(-113);
      break;
    case 'k':
      if (obstleft()) BTSerial.println("Obstacle left");
      delay(500);
      break;       
    case 'l':
      if (obstright()) BTSerial.println("Obstacle right");
      delay(500);
      break;
     case 'g':
         dspgyro.getGyroSettings(true);
         break;
     case 'u':   
         dspgyro.goLeft(90);
         break;
     case 'i':
         dspgyro.goRight(90);
         break;        
     case 'b':
         BTSerial.print("Distance traveled:");
         BTSerial.println(Myrobo.mydist,DEC);   
         BTSerial.println(Myrobo.inCM(Myrobo.distanceTravelled),DEC);
        break;
     case 'n':
         Myrobo.clearDistance();
        break;
    }
  }

  
  switch (Myrobo.robomode) {
  case progload:
    smode="Prog";                    
    DoProgram();
    break;
  case rc:
    smode="RC 1";
    RCloop();
    break;
  case rc2:
    smode="RC 2";
    RCloop2();
    break;
  case robot:
    smode="ROBO";
    Robotloop();
    break;
  case robotbl:
    smode="RBBL";
    Robotloop();
    break;           
  }
                            
  if (beardist!=0) {                                            
    
                     
    if ( (!dspgyro.turning && Myrobo.RoboState()!=rcStop) ||
                                                      
       (!((lastbeardist<0 && beardist<0) or (lastbeardist>0 && beardist>0)) ) )  Myrobo.Stop(true);
   
                                     
    if (!dspgyro.turning || Myrobo.RoboState()==rcStop) {
     
     Serial.print("turning:");
     Serial.println(beardist,DEC);
       if (abs(beardist)>100)
        turndeg=60;          
       if (abs(beardist)>60)
        turndeg=30;     
       else 
       if (abs(beardist)>30)
        turndeg=20;     
       else
       if (abs(beardist)>15)
        turndeg=10;             
       else{
        turndeg=1;
        
       }

     if (beardist<0) 
        Myrobo.turnRight(turndeg);    
     else 
        Myrobo.turnLeft(turndeg);       
    lastbeardist=beardist;
    }
  }   


 

  if (millis()-tm3>300) {    
    showinfo1(); 
                          
                        

    tm3=millis();
  }

  if (((millis()-tm2)>500) ){
    showinfo0();
    showBLinfo();
    tm2=millis();
  }

  if (millis()-tm4>600) {      
    lcd.setCursor(4, 1);      
                           
    lcd.print(int(dspgyro.curbearing),DEC);
    lcd.print(" ");
    lcd.print(int(dspgyro.targbearing),DEC);
    lcd.print("  ");
    lcd.print(int(beardist),DEC);
    lcd.print("  ");
    
    tm4=millis();
  }

}










