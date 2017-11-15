
#include <DspMotor.h>
#include <DspExpander.h>
#include <Wire.h>    // Required for I2C communication
#include <PCF8575.h> // Required for PCF8575
#include "SoftwareSerial.h"// import the serial library
#include <LiquidCrystal.h> //lcd library
#include <IRremote.h>


int RECV_PIN = 3; //define input pin on Arduino

IRrecv irrecv(RECV_PIN); //Irreceiver


//DESP_DCMotor motor1(1,2,3);  //back
//DESP_DCMotor motor2(2,5,4);
DESP_DCMotor motor1;  //back
DESP_DCMotor motor2;  //back

DESP_DCMotor motor3;  //front
DESP_DCMotor motor4;//front

// BlueTooth serial comm
SoftwareSerial BTSerial(6, 7); // TX, RX
/** PCF8575 instance */
//PCF8575 expander;
PCF8575 expander;

LiquidCrystal lcd(13, 12, 8, 9, 10, 11); //RS, ,D4,D5,D6,D7

//sonic sensor
#define trigPin 4
#define echoPin 5
long distanceF=5000;
//IR obstacle Sensor
#define obstacleL 1  //expanded
#define obstacleR 0  //expanded
boolean obstacleLeft=false;
boolean obstacleRight=false;

#define BlackLine1 7  //expanded
#define BlackLine2 5  //expanded
#define BlackLine3 6  //expanded
#define BlackLine4 4  //expanded
#define BlackLine5 3  //expanded

enum RoboMode {
  progload,
  rc,
  rc2,
  robot  
};

RoboMode robomode=rc2;

String comnd;

enum BLLine {
  blFarLeft,
  blLeft,
  blFront,
  blRight,
  blFarRight,
  blNone
};
BLLine bln=blNone;

void setup() {

 
  
  Serial.begin(9600); // Enables Serial monitor for debugging purposes
  Serial.println("Serial comm ok!"); // Test the Serial communication

  irrecv.enableIRIn(); // Start the receiver

  motor1.init(1,epin(15),epin(13));
  
  motor2.init(2,epin(9),epin(11));
  motor3.init(3,epin(14),epin(12));
  motor4.init(4,epin(8),epin(10));  

  
  

//bluetooth serial
  BTSerial.begin(9600);
  BTSerial.println("Bluetooth On please press 1 or 0 blink LED ..");
 
 //lcd
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("LCD OK....");
  
  
 //sonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
 //iR obstacle Sensor
  expander.pinMode(obstacleL, INPUT);
  expander.pinMode(obstacleR, INPUT);  
  expander.pullUp(obstacleL);
  expander.pullUp(obstacleR);
    
 //Line detector   
  expander.pinMode(BlackLine1, INPUT);
  expander.pinMode(BlackLine2, INPUT);  
  expander.pinMode(BlackLine3, INPUT);
  expander.pinMode(BlackLine4, INPUT);  
  expander.pinMode(BlackLine5, INPUT);
  
    expander.pullUp(BlackLine1);
    expander.pullUp(BlackLine2);
    expander.pullUp(BlackLine3);
    expander.pullUp(BlackLine4);
    expander.pullUp(BlackLine5);


 Serial.println("setup ok!"); // Test the Serial communication
}




void checkBlackLine(){
  boolean ss1,ss2,ss3,ss4,ss5;
  
  
  ss1=expander.digitalRead(BlackLine1)==LOW; //ok
  ss2=expander.digitalRead(BlackLine2)==LOW; //ok
  ss3=expander.digitalRead(BlackLine3)==LOW; //ok
  ss4=expander.digitalRead(BlackLine4)==LOW; //ok
  ss5=expander.digitalRead(BlackLine5)==LOW; //ok
  
 // BTSerial.println("BLACKLINES"); 
 // BTSerial.println(ss1); 
 // BTSerial.println(ss2); 
 // BTSerial.println(ss3); 
 // BTSerial.println(ss4);   
 // BTSerial.println(ss5);   
  
  bln= blNone;

  if ((ss4 and !ss3 and !ss2) or (ss4 and ss3 and !ss2) or (ss4 and !ss3 and ss2))
    bln= blRight;
  if ((!ss4 and ss3 and !ss2) or (ss4 and ss3 and ss2))
    bln= blFront;
  if ((!ss4 and !ss3 and ss2) or (!ss4 and ss3 and ss2))
    bln= blLeft;
  if (ss5) bln=blFarRight;
  if (ss1) bln=blFarLeft;
  
 
}


decode_results results;

int getIRbutton(){
  
if (irrecv.decode(&results)) {

 //Serial.println(results.value, HEX);

 irrecv.resume(); // Receive the next value
// delay(1000);
 return results.value;
}  
else return 0;
}

int getDistance(){
  
long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  return  (duration/2) / 29.1;  
}



unsigned long tm3=0;
unsigned long tm4=0;

void checkSensors(){
  
   //check Left obstacle
   obstacleLeft=expander.digitalRead(obstacleL)==LOW;
//   if (obstacleLeft) 
//     Serial.println("Obstacle at left side");
     
   
   //check Left obstacle
   obstacleRight=expander.digitalRead(obstacleR)==LOW;
//   if (obstacleRight) 
//     Serial.println("Obstacle at right side");    
   
  //check front distance
  if (millis()-tm4>80){
     distanceF=getDistance();
   //  Serial.println(distance);
     tm4=millis();

   }

  if (millis()-tm3>200) {
    tm3=millis();
    showinfo1(); 
  }
  
  checkBlackLine();
}

void Backward(){
  Stop();
 motor1.run(FORWARD);
 motor2.run(FORWARD);
 motor3.run(FORWARD);
 motor4.run(FORWARD);
 comnd="BACK";
}

void Forward(){
 Stop();
 motor1.run(BACKWARD);
 motor2.run(BACKWARD);
 motor3.run(BACKWARD);
 motor4.run(BACKWARD);
 comnd="FORW"; 
 
}

void Stop(){
  
 motor1.run(RELEASE);
 motor2.run(RELEASE);
 motor3.run(RELEASE);
 motor4.run(RELEASE);
  comnd="STOPPED"; 
}

void left(){
  Stop();
 motor1.run(FORWARD);
 motor2.run(BACKWARD);
 motor3.run(BACKWARD);
 motor4.run(FORWARD);
  comnd="LEFT";
}

void right(){
  Stop();
 motor1.run(BACKWARD);
 motor2.run(FORWARD);
 motor3.run(FORWARD);
 motor4.run(BACKWARD);
 
  comnd="RIGHT";
}

boolean frontIsClear(){
  
 return distanceF>15; 
}







// Starts the loop to decide what to do
char lastcmd=' ';


void RCloop(){
  
  if (!frontIsClear()) Stop();
  
 switch (lastcmd){
   case 'w':
     if (frontIsClear())
        Forward();              
           break;
   case 's':
         Backward();
           break;
   case ' ':
      case '*':
     Stop();
         break;
   case 'a':
         left();
           break;
   case 'd':
         right();
           break;
         
         
   case 't':
      if(lastcmd=='1'){   // if number 1 pressed ....
           expander.digitalWrite(3, HIGH); // Turn on led 1
           BTSerial.println("LED  On EXP P03 ON ! ");
         }
      if (lastcmd=='0'){// if number 0 pressed ....
           expander.digitalWrite(3, LOW); // Turn off led 1
           BTSerial.println("LED  On EXP P03 Off ! ");
        }  
      break;
 }
 
  
}


int steptime=1000; //millis
int degtime=6; //millis
int degs=90;
unsigned long tm1;
int moving=0;

void stepForward(){
  
  tm1=millis();
  moving=1;
  Forward();
  
}

void stepBackward(){
  tm1=millis();
  moving=2;
  Backward();
  
}

void turnLeft(int deg){
  tm1=millis();
  moving=3;
  degs=deg;
  left();
}

void turnRight(int deg){
  tm1=millis();
  moving=3;
  degs=deg;  
  right();

}

boolean movingForw(){
 return moving==1;  
}

boolean movingBack(){
 return moving==2;  
}

boolean turning(){
 return moving==3;  
}



boolean timeisup(){
  
  return ((movingForw() || movingBack()) && ((millis()-tm1)>steptime)) || (turning() && ((millis()-tm1)>(degs*degtime))) ;
}

void RCloop2(){
  
 if (!frontIsClear() && movingForw()) {
    Stop();
    moving=0;
   Serial.println("Stopped not clear ahead!!!");  
}
 if (timeisup()) {
   Stop();
   moving=0;
   Serial.println("Stopped time is up1!!!");   
 }
  
  
 switch (lastcmd){
   case 'w':
     if (frontIsClear())
        stepForward();              
           break;
   case 's':
         stepBackward();
           break;
   case ' ':
   case '*':
     moving=0;
     Stop();
         break;
   case 'a':
         turnLeft(45);
           break;
   case 'd':
         turnRight(45);
           break;
         
  }
 
  
}

String Err="";

void Robotloop(){

 if (!frontIsClear() && (movingForw())) {
    Stop();
    moving=0;
   Serial.println("Stopped not clear ahead!!!");  
 }
  
  if (  timeisup()) {
   Stop();
   moving=0;
   Serial.println("Stopped time is up!!!");   
 }
  
  if (moving==0) {
  
    if (bln!=blNone){
      switch (bln){
       case blLeft: 
            turnLeft(15);
            break;
       case blFarLeft: 
            turnLeft(35);
            break;
       case blRight: 
            turnRight(15);
            break;
       case blFarRight: 
            turnRight(35);
            break;            
       case blFront: 
            if (frontIsClear()){
             Err="";
             stepForward();
            }
            break;
       
      } 
      
    }
    else
    {
      if (frontIsClear()){
        Err="";
        stepForward();
      }
      else
      if (!obstacleLeft) turnLeft(45);
       else
        if (!obstacleRight) turnRight(45);
         else {
          Err="No way Out. Halted.";      
          Stop();
         }

  
    }//bln!=blnone
  }//moving=0
  
  
  
}


void showinfo1(){
 
  lcd.setCursor(0, 1);
  lcd.print("               ");
  lcd.setCursor(0, 1);
  
  lcd.print("d:");
  lcd.setCursor(3, 1);  
  lcd.print(distanceF);
   
  if (obstacleLeft) {
    lcd.setCursor(10, 1);      
    lcd.print("L");  
  }

  if (obstacleRight) {
    lcd.setCursor(12, 1);  
    lcd.print("R");  
  }
  

  lcd.setCursor(14, 1);  
  lcd.print(moving);  
}

String smode;

void showinfo0(){
  lcd.setCursor(0, 0);
  lcd.print("                 ");
  lcd.setCursor(0, 0);
  if (Err!="") 
   lcd.print(Err);     
  else { 
    lcd.print(smode);
    lcd.setCursor(6, 0); 
   Serial.println(comnd);   
    lcd.print(comnd);
    lcd.setCursor(12, 0);    
    lcd.print(lastcmd);
    if (bln!=blNone){
      lcd.setCursor(14, 0);    
      lcd.print("BLN");
      
    }
    
  }
}

void showBLinfo(){
  
   switch (bln){
    
    case blNone:
       BTSerial.println("No BlackLine");
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

void setNextMode(){
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
           robomode=progload;
           break;
  }
  
}

int lastmove=0;

char getcommand(){
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
 if (c!='~'){
   Serial.print("IR button: ");   
  Serial.println(c);   
 }
 return c;
}


void loop()
{
   
   checkSensors();
   

   //get bluetooth command
   //lastcmd=BTSerial.read(); //bluetooth comm
 //  lastcmd=Serial.read();  //USB comm
  
   lastcmd=getcommand();// IR comm
   if (lastcmd='~') 
      lastcmd=BTSerial.read();
   
 
  if (lastcmd!='~' and int(lastcmd)!=-1) {
    BTSerial.println(lastcmd);
    Serial.println(int(lastcmd));
    if (lastcmd=='m') { //change mode of operation
      setNextMode();
      
    }
    switch (lastcmd) {
       case '1':
           robomode=progload;
           break;
       case '2':
           robomode=rc;
           break;
       case '3':
           robomode=rc2;
           break;
       case '4': 
           robomode=robot;
           break;     
      
    }
  }
  
  switch (robomode) {
   case progload:
           smode="Prog";
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
  }
  
  if (((millis()-tm2)>400) or (lastmove!=moving)){
    showinfo0();
    showBLinfo();
    tm2=millis();
  }
  lastmove=moving;
}








