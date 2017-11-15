#ifndef newcar_h
#define newcar_h

#include <arduino.h>
#include <Servo.h>
#include <DspRobot.h>
#include <DspSensors.h>

Servo servo;
DESP_Robot Myrobo;
DESP_Sensors Mysensors;

#include "SoftwareSerial.h"// import the serial library
#include <LiquidCrystal.h> //lcd library

// BlueTooth serial comm
SoftwareSerial BTSerial(6, 7); // TX, RX
LiquidCrystal lcd(13, 12, 8, 9, 10, 11); //RS, ,D4,D5,D6,D7


String comnd;
char lastcmd=' ';
String Err="";


//robot_logic
int RECV_PIN = A5; //define input pin on Arduino for IR Receiver  was 3


//BlackLine Sensor
#define BlackLn1 23  
#define BlackLn2 25  
#define BlackLn3 27  
#define BlackLn4 29  
#define BlackLn5 31 
#define frontSensor 33  

//for 28byj NOT USED
#define sensmotor1 22
#define sensmotor2 24
#define sensmotor3 26
#define sensmotor4 28

//motor pins
#define mot1p1 48
#define mot1p2 46
#define mot2p1 50
#define mot2p2 52

#define mot3p1 49
#define mot3p2 47
#define mot4p1 51
#define mot4p2 53

#define motEN 30

//speed encoder pin and interrupt no
#define speedenc 19
#define speedint 4

//Sonic Sensor
int trPin=5;//4;
int ecPin=4;//5;

//gyro
int maxdeviation=4;//when turning how close to target we consider ok (degrees)


#endif


