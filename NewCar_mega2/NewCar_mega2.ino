#include <arduino.h>
#include "NewCar_mega.h"

#include <I2Cdev.h>    // Required for I2C communication
#include <Wire.h>    // Required for I2C communication
#include <PCF8575.h> // Required for PCF8575

#include <NewPing.h>
#include <DspMotor.h>
#include <DspRobot.h>
#include <DspSensors.h>
#include <DspGyro.h>
#include <DspInterpreter.h>;
#include "SoftwareSerial.h"// import the serial library
#include <LiquidCrystal.h> //lcd library
#include <Servo.h>

#include "NewCar_func.cpp"
#include "NewCar_loops.cpp"





//DESP_DCMotor motor;




void setup() {



  Serial.begin(115200); // Enables Serial monitor for debugging purposes
  Serial.println("Serial comm ok!"); // Test the Serial communication

    //bluetooth serial
  BTSerial.begin(9600);
  BTSerial.println("Bluetooth On  ..");

  //lcd
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("LCD OK....");
  delay(1000);

  Mysensors.setSonicParams(25,100,trPin,ecPin);//distancemin,checkevery
  Mysensors.setBlacklineParams(BlackLn1,BlackLn2,BlackLn3,BlackLn4,BlackLn5,frontSensor);
  Mysensors.init();

  lcd.clear();
  lcd.print("Sensors OK....");
  delay(1000);

  //ROBO Stuff  
  Myrobo.init(motEN,mot1p1,mot1p2,mot2p1,mot2p2,mot3p1,mot3p2,mot4p1,mot4p2 );
  Myrobo.stepMoveTime=200;//msecs for 1 step
  Myrobo.stepTurnTime=25;//msecs for 1 degree
  lcd.clear();
  lcd.print("Robo OK....");
  delay(1000);

  servo.attach(A0);
  servo.write(180);delay(500);
  servo.write(0);delay(500);
  servo.write(90);delay(500);
  

  Serial.println("setup ok!"); //  Serial communication
}





//Main LOOP 


unsigned long tm2=0;
unsigned long tm3=0;


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
      servo.write(15);
      break;       
    case 'i':
      servo.write(105);
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









