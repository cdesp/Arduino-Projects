#include <NewPing.h>


#include <DspExpander.h>
#include <DspMotor.h>;
#include <DspRobot.h>;
#include <DspSensors.h>;
//#include <IRremote.h> //IR remote control
#include <DspStepper.h>;


#include <Wire.h>    // Required for I2C communication
#include <PCF8575.h> // Required for PCF8575
#include "SoftwareSerial.h"// import the serial library
#include <LiquidCrystal.h> //lcd library
/** PCF8575 instance */
PCF8575 expander;
// BlueTooth serial comm
SoftwareSerial BTSerial(6, 7); // TX, RX
LiquidCrystal lcd(13, 12, 8, 9, 10, 11); //RS, ,D4,D5,D6,D7

DESP_DCMotor motor;

//robot_logic
int RECV_PIN = A5; //define input pin on Arduino for IR Receiver  was 3

//IR obstacle Sensor
#define obstacleL 2 
#define obstacleR 3 

//BlackLine Sensor
#define BlackLine1 2  //expanded
#define BlackLine2 epin(5)  //expanded
#define BlackLine3 epin(7)  //expanded
#define BlackLine4 epin(3)  //expanded
#define BlackLine5 2  //expanded
#define frontSensor A3  //expanded

#define sensmotor1 epin(0)//2
#define sensmotor2 epin(2)//3
#define sensmotor3 epin(4)//4
#define sensmotor4 epin(6)//5




//Sonic Sensor
int trPin=5;//4;
int ecPin=4;//5;

String comnd;

DESP_Robot Myrobo;
DESP_Sensors Mysensors;

char lastcmd=' ';

DESP_Stepper stepmotor;



void setup() {



  Serial.begin(9600); // Enables Serial monitor for debugging purposes
  Serial.println("Serial comm ok!"); // Test the Serial communication

    //bluetooth serial
  BTSerial.begin(9600);
  BTSerial.println("Bluetooth On please press 1 or 0 blink LED ..");

  //lcd
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("LCD OK....");
  delay(1000);

  Mysensors.setSonicParams(25,100,trPin,ecPin);//distancemin,checkevery
  Mysensors.setIRObstacleParams(obstacleL,obstacleR);
  Mysensors.setBlacklineParams(BlackLine1,BlackLine2,BlackLine3,BlackLine4,BlackLine5,frontSensor);
  Mysensors.init();

  lcd.clear();
  lcd.print("Sensors OK....");
  delay(1000);

  //ROBO Stuff  
  Myrobo.init(epin(1));
  Myrobo.stepMoveTime=200;//msecs for 1 step
  Myrobo.stepTurnTime=25;//msecs for 1 degree
  lcd.clear();
  lcd.print("Robo OK....");
  delay(1000);

  stepmotor.init(sensmotor1, sensmotor2, sensmotor3, sensmotor4);  
  //  stepmotor.setSpeed(20);
  stepmotor.DoTurn(500);
  stepmotor.DoTurn(-1000);
  stepmotor.DoTurn(500);  

  Serial.println("setup ok!"); //  Serial communication
}



//continuous movement
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
    Myrobo.Stop();
    break;
  case 'a':
    Myrobo.left();
    break;
  case 'd':
    Myrobo.right();
    break;       
  }  
}


//Step movement
String mes1;

void RCloop2(){

  checkobstacle();

  mes1=Myrobo.checkStatus();//checks for movement timeout
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
    Myrobo.Stop();
    break;
  case 'a':
    Myrobo.turnLeft(45);
    break;
  case 'd':
    Myrobo.turnRight(45);
    break;

  }


}


//Robot self guidance
String Err="";

void Robotloop(){

  //   BTSerial.println("robo turn Start ======");
  //  BTSerial.println("checkobst");
  checkobstacle();

  if (Myrobo.RoboState()==rcStop) { //check for black line
    //  BTSerial.println("rcStop");
    if ((Myrobo.robomode!=robot) && (Mysensors.blLine !=blNone)){
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
      // BTSerial.println("check Moving");
      if (Mysensors.frontIsClear()){
        Err="";
        Myrobo.stepForward();
      }
      else
      {
        //   BTSerial.println("Stop check left right");
        Myrobo.Stop();
        if (!obstleft()) Myrobo.turnLeft(20);
        else
          if (!obstright()) Myrobo.turnRight(20);
          else {
            Err="No way Out. Halted.";  
            Myrobo.stepBackward();          
          }
        //     BTSerial.println(Mysensors.distanceFront());  
      }//frontisclear

    }//bln!=blnone
  }//moving=0
  Mysensors.checkStatus();
  // BTSerial.println("robo turn end======");
  //  BTSerial.println("");
}

boolean obstleft(){
  boolean ret;

  stepmotor.DoTurn(1000);
  Mysensors.checkStatus();
  delay(200);
  Mysensors.checkStatus();
  showinfo1(); 
  BTSerial.println(Mysensors.distanceFront());
  ret= !Mysensors.frontIsClear(); 
  stepmotor.DoTurn(-1000);
  return ret;
}

boolean obstright(){
  boolean ret;

  stepmotor.DoTurn(-1000);
  Mysensors.checkStatus();
  delay(200);
  Mysensors.checkStatus();
  showinfo1(); 
  BTSerial.println(Mysensors.distanceFront());
  ret= !Mysensors.frontIsClear(); 
  stepmotor.DoTurn(1000);
  return ret;
}

void checkobstacle(){
  Myrobo.checkStatus();
  Mysensors.checkStatus();

  if (!Mysensors.frontIsClear() && (Myrobo.RoboState()==rcForward)) {
    Myrobo.Stop();

    Serial.println("Stopped not clear ahead!!!");  
  } 
  if (Mysensors.frontBumperHit()){

    Myrobo.Stop();
    Myrobo.stepBackward();
  } 
}

//Information 
void showinfo1(){

  lcd.setCursor(0, 1);
  lcd.print("               ");
  lcd.setCursor(0, 1);

  lcd.print("d:");
  lcd.setCursor(3, 1);  
  lcd.print(Mysensors.distanceFront());

  //  if (Mysensors.obstacleLeft()) {
  //    lcd.setCursor(10, 1);      
  //    lcd.print("L");  
  //  }

  //  if (Mysensors.obstacleRight()) {
  //    lcd.setCursor(12, 1);  
  //    lcd.print("R");  
  //  }


  // lcd.setCursor(12, 1);  
  // lcd.print(Myrobo.getCurrentCommand());  
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
    //   if (comnd!="")
    //     Serial.println("CMND:["+comnd+"]");   
    lcd.print(comnd);
    lcd.setCursor(12, 0);    
    lcd.print(lastcmd);
    if (Mysensors.blLine!=blNone){
      lcd.setCursor(14, 0);    
      lcd.print("BLN");

    }

  }
}

void showBLinfo(){
  if (Myrobo.robomode==robotbl)
    switch (Mysensors.blLine){

    case blNone:
      //BTSerial.println("No BlackLine");
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


//Main LOOP 


unsigned long tm2=0;
unsigned long tm3=0;

void loop3()
{



  //get bluetooth command
  //lastcmd=BTSerial.read(); //bluetooth comm
  //  lastcmd=Serial.read();  //USB comm
  if (Serial.available()>0)
  {
    int steps = Serial.parseInt();
    stepmotor.DoTurn(steps);
    Serial.println("ok");
  }  

}

void loop()
{


  checkobstacle();


  //get bluetooth command
  //lastcmd=BTSerial.read(); //bluetooth comm
  //  lastcmd=Serial.read();  //USB comm

  lastcmd='~';
  //lastcmd=Mysensors.getIRcommand();// IR comm   
  if (lastcmd=='~') lastcmd=BTSerial.read();
  if (int(lastcmd)==-1) lastcmd=Serial.read();


  if ((lastcmd!='~') && (int(lastcmd)!=-1)) {
    BTSerial.println(lastcmd);
    Serial.println(int(lastcmd));

    switch (lastcmd) {
    case '1':
      Myrobo.robomode=progload;
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
    case 'u':
      stepmotor.DoTurn(150);
      break;       
    case 'i':
      stepmotor.DoTurn(-150);
      break;
    case 'k':
      if (obstleft()) BTSerial.println("Obstacle left");
      delay(500);
      break;       
    case 'l':
      if (obstright()) BTSerial.println("Obstacle right");
      delay(500);
      break;



    }
  }

  switch (Myrobo.robomode) {
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
  case robotbl:
    smode="RBBL";
    Robotloop();
    break;           
  }

  checkobstacle();

  comnd=Myrobo.getCurrentCommand();

  if (millis()-tm3>300) {
    tm3=millis();
    showinfo1(); 
  }

  if (((millis()-tm2)>500) ){
    showinfo0();
    showBLinfo();
    tm2=millis();
  }

}









