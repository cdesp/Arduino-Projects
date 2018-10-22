#ifndef ctrlpl_h
#define ctrlpl_h

#include <NewPing.h>
#include <DspMotor.h>
#include <DspInterpreter.h>                          
#include <helper_3dmath.h>
#include "MPU6050_6Axis_MotionApps20.h"
#include <DspGyro.h>
#include <I2Cdev.h>
#include <Wire.h>                                     
#include "SoftwareSerial.h"                                
#include <Speaker.h>
#include <Servo.h>
#include "CPSonicSensor.h"
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <SFE_BMP180.h>
#include "CPRobot.h" //TODO: Should control 2 DCMotors and a Gyro for turning Right and left by certain degrees. Also a photointerrupter to count distance travelled.
                    //Commands: Forward,backward,Stop, StepForward,StepBackward, Turn Left xx degrees, Turn Right xx degrees.




// Defines for LCD Serial
#define I2C_ADDR          0x27        //Define I2C Address where the PCF8574A is
#define BACKLIGHT_PIN      3
#define En_pin             2
#define Rw_pin             1
#define Rs_pin             0
#define D4_pin             4
#define D5_pin             5
#define D6_pin             6
#define D7_pin             7


//devices support
typedef struct actdevice
{
  int8_t actdevid; //signed byte
  int param[4];
  uintptr_t  devpointer;
} actdevice;

actdevice actdevices[10];
int actdevcount=0;


//Supported Devices
LiquidCrystal_I2C *lcd=NULL;  //Serial LCD                   
SoftwareSerial *BTSerial=NULL; //BlueTooth Serial comm
NewPing *sonarf=NULL; //UltraSonic Sensor
CP_SonicSensor *sonic=NULL; //UltraSonic Sensor
DESP_Gyro *dspgyro=NULL; //Gyroscope 
Servo *serv=NULL; //MicroServo SG90
Speaker* speak=NULL; //Buzzer just a pin
DHT*  dht=NULL; //temp sensor DHT
SFE_BMP180* bmp=NULL; //Barometric Sensor SDA/SCL
double relP,Pres,bmpTemp;  //Pres=Absolute Pressure, relP= Pressure at sea level, bmpTemp=Temperature
DESP_DCMotor* Motor=NULL;//DC Motor
int motid=0;//Motor ID auto increases for each desp_dcmotor probably not needed
CP_Robot* Robo=NULL;//Cp Robot (Contains 2 motors,1 gyroscope and a photointerrupter)

// 
DESP_Interpreter interpreter;

enum TPlatfMode {
  none,
  progload
};                                                                                        

TPlatfMode PlatfMode;
TPlatfMode prevmode;

int lastcmdid=-1;
boolean lastcommandexecuted=true;
DspCommand *cmd;
int setupcmds;

int ChkByte=0;//for crc checking of input bytes

boolean inprogram=true;

//String mes1;
//String Err="";         
String smode;


char lastcmd=' ';
char nextcmd=' ';

#endif


