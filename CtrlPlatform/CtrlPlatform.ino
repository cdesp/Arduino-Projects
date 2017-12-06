#include <Arduino.h>
#include "CtrlPlatform.h"

                                   
void speedencint()//attach change photointerrupt here (Wrapper function for my class)
{
  if (Robo)
       Robo->handleInterrupt();   
}


void LCDprint(String s){
  if (lcd!=NULL) {
     lcd->print(s);
  }
}  

void LCDclear(){
  if (lcd!=NULL) {
    lcd->clear(); 
  }
}

void BTprint(String s){
 // if (BTSerial!=NULL) BTSerial->println(s);
   Serial.println();
}

int BTread(){
 // if (BTSerial!=NULL)  return BTSerial->read();
  return Serial.read();
}

int BTavailable(){
 // if (BTSerial!=NULL) return BTSerial->available();
    return Serial.available();
}

//int BToverflow(){
// if (BTSerial!=NULL) return BTSerial->overflow();
//   else return false;//Serial.overflow();
//}   

void BTwrite(uint8_t b){
// if (BTSerial!=NULL) BTSerial->write(b);
Serial.write(b);
}

                                                                   
void DoDebug(String s,int n){
 // if (lcd==NULL) {
   Serial.print(s);
   Serial.println(n,DEC);
  //}
  //else
  //{
//    LCDclear();
//    LCDprint(s);
//    LCDprint(" ");
//    LCDprint(String(n));
//  }
}

unsigned long tm9=0;
void myremdebug(String s){
  if (millis()-tm9>200) {
   BTprint(s);  
   tm9=millis();
  }
}


//-----------------------------------------
//SETUP DEVICES ON DEMAND
//-----------------------------------------

void setuplcd(){   
  //Initialise the LCD
  
  if (lcd==NULL){
   lcd = new LiquidCrystal_I2C(I2C_ADDR, En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);
   lcd->begin(16, 2);
   //Switch on the backlight
   lcd->setBacklightPin(BACKLIGHT_PIN,POSITIVE);
   lcd->setBacklight(HIGH);  
 //  LCDprint(F("LCD OK...."));
 //  delay(2000); 
  }
}

void setupBT(int txpin,int rxpin){//12,13
  BTSerial= new SoftwareSerial(txpin, rxpin);
  BTSerial->begin(9600);
  BTSerial->flush();
//  BTSerial->println(F("Bluetooth On ..."));  
}

void setupGyro(){
  dspgyro= new DESP_Gyro(MPU6050_DEFAULT_ADDRESS);
  dspgyro->RemDebug=myremdebug;
  dspgyro->init();
}

void setup() {

  Serial.begin(9600);                                                 
  Serial.println(F("Serial comm ok!"));                                 
  
  interpreter.evaluate=evalfunc;  
  interpreter.evalvar=evalvarfunc;
   //interpreter.setfuncif(isIFfunc);  //TODO:uncomment to see if its ok 

//  setupGyro(); //Gyroscope eats a lot of memory
}


//get remote programming
                           

int converttoint(byte b1,byte b2){
  return b2*256+b1;   
}

int getNumberFromBT(){
 byte b1,b2;

 while (!BTavailable()){ 
 // if (BToverflow()) {
 //  Serial.println(F("SoftwareSerial overflow!")); 
  //}
 }
 
 b1=BTread();           
 while (!BTavailable()) {
 // if (BToverflow()) {
//   Serial.println(F("SoftwareSerial overflow!")); 
//  }
 }
 b2=BTread();         
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


char getCharFromBT(){
 byte b1;

 while (!BTavailable()){ 
 // if (BToverflow()) {
 //  Serial.println(F("SoftwareSerial overflow!")); 
 // }
 }
 
 b1=BTread();           
 if (b1!=-1) 
  ChkByte = ChkByte ^ b1;

 return b1;
}

boolean isDeviceSetupCmd(int cmdid)
{
  switch (cmdid) {
    case 50: //Robo
    case 60:   //DCmotor
    case 110:  //Sound
    case 120:  //LCD
    case 130:  //Laser   
    case 140:  //ultrasonic device
    case 145: //switch
    case 150: //Servo
    case 160: //Photointerrupter not working yet   
    case 165://Moisture 
    case 170: //temp DHT
    case 176://BMP180 barometric
        return true;
  }

  return false;
}

void addDevice(int devid)
{ 
// DoDebug(F("Adding Device id="),devid); 
// DoDebug(F("Position="),actdevcount); 
 actdevices[actdevcount++].actdevid=devid;  
}

actdevice* getDeviceById(int devid)
{
  for (int i=0;i<actdevcount;i++)
    if (actdevices[i].actdevid==devid) {
     //  DoDebug(F("DEVICE FOUND="),i);
     //  DoDebug(F("DEVICE Addr="),(int)(actdevices+i));
       return actdevices+i;
    }

//  DoDebug(F(" NULL DEVICE NOT FOUND:"),devid);
//  for (int i=0;i<actdevcount;i++)
 //   DoDebug(F("Device:"),actdevices[i].actdevid);     
  return NULL;  
}


void setDevicePin(int devid,int paramno,int paramvalue)
{
 if (devid>=0)  
   getDeviceById(devid)->param[paramno-1]= paramvalue;
// else
//   DoDebug(F("SET Invalid Device "),devid);
}

int getDevicePin(int devid,int paramno)
{
 if (devid>=0)    
   return getDeviceById(devid)->param[paramno-1];
// else
 //  DoDebug(F("GET Invalid Device "),devid);
}


boolean getProgramFromBT()
{
  int siz,t,strsize;
  int i=0;
  int c,p1,p2,vv;
  int8_t v1,v2,did;
  int predid=-1;
  int myprog[50];
  int mpi=0;
  DspCommand* DC;
  char r;
  String err;

  actdevcount=0;//reset device table
 // err="";
 // DoDebug(F("Get new program"),0);
  //BTwrite("In Programming..          ");  
  t= millis();
  do{
     BTwrite('R'); 
     delay(50);
  }
  while ((BTavailable()==0) and (millis()-t<5000));
  
  siz=BTread();
 // DoDebug("Prog size:",siz);
  
  if (siz<1) {
    // Serial.println(F("Invalid Size. abort"));
    PlatfMode=none;                   
    return false;
  }

 // if (BTSerial==NULL) Serial.println(F("Read Program"));
  delay(400);
  interpreter.newProgram();
  ChkByte=0;
  do
  {
    c=getNumberFromBT();
    if (c!=253){
      vv=getNumberFromBT();
      did=vv & 0xff;
      p1=getNumberFromBT();
      vv=getNumberFromBT();  //get var param
      v1=vv & 0xff;
      vv=getNumberFromBT();  //get var param
      v2=vv & 0xff;      
      p2=getNumberFromBT();  //always last because of the string param
      
    }    
    if (BTSerial==NULL) {
       Serial.print("Inp Byte: ");
       Serial.print(c,DEC);    
       Serial.print(" ");     
       Serial.print(did,DEC);    
       Serial.print(" ");                   
       Serial.print(p1,DEC);  
       Serial.print(" ");            
       Serial.print(v1,DEC);        
       Serial.print(" ");            
       Serial.print(v2,DEC);        
       Serial.print(" ");            
       Serial.println(p2,DEC);        
       
       
    }
   else
     {
//      myprog[mpi]=c;mpi++;
//      myprog[mpi]=p1;mpi++;
//      myprog[mpi]=p2;mpi++;     
     }
    if ((c!=253) && (c!=-1)) {
      DC=interpreter.addCommand(c,p1,p2,did);  
      DC->var1idx=v1;
      DC->var2idx=v2;     
      if (interpreter.isStringCommand(c)) {
        //fill the string param        
        DC->paramstr=new char[p2+1];//param2 has the size of the string
       // DC->paramstr = (char *)malloc(p2+1);
        //next we should get the chars from serial port
        for (i=0;i<p2;i++){
           r=getCharFromBT();
           DC->paramstr[i]=r;
//           myprog[mpi]=r;mpi++;
        } 
       //DC->paramstr[p2]='\0';
      }
      if (isDeviceSetupCmd(c)){ //add new device if the command is setup a device
        if (predid!=did) {//if we send 3 or 4 params for the device initilization skip
          predid=did;
          addDevice(did);//fill other param pins on execution of the setup        
        }
      }
    }        
  } while (c!=253);                  
  BTwrite('R'); 
  do {
    c=BTread();
  } while ((c==253) or (c==-1));
  
  if (ChkByte==253) ChkByte=254;
  
  int testchkbyte=c;  
  //send back 
//  for (i=0;i<mpi;i+=3){
//     Serial.print(myprog[i]);Serial.print(" ");      
//     Serial.print(myprog[i+1]);Serial.print(" ");      
//     Serial.print(myprog[i+2]);Serial.println(" ");      
//  }
//  Serial.print(testchkbyte);Serial.print(" ");Serial.print(ChkByte);
  
  if (testchkbyte==ChkByte){  
   // Serial.println(F("Program Read ok"));
   // Serial.println(err);
  //  BTprint(F("Program Transfer ok"));
  }
  else   {
   // Serial.println(F("CRC Error!!! "));
  //  Serial.println(testchkbyte,DEC);Serial.println(" ");
  //  Serial.println(ChkByte,DEC);Serial.println(" ");
   // Serial.println(err);
    //BTwrite("CHECKSUM ERROR");
    delay(2000);
    return false;
  }
  delay(1000);
  while (BTavailable()){                    
    BTread();    
  }
  return true;
     
}


int evalvarfunc(DspCommand *cmd,int varidx){

  int devid;
   actdevice* devce; 
  
//  DoDebug(F("FIX VAR:"),varidx);
//  DoDebug(F("CMDID:"),cmd->cmdid);
//  DoDebug(F("DEVID:"),cmd->devid);

//varidx for FIX VARS is 75..90 because of signed byte -127..128
  switch(varidx){//we need the device ID also ????
     case 75: //Evaluate temperature on device cmd->devid 
              devce=getDeviceById(cmd->devid);  
              dht=(DHT*)((void*)(devce->devpointer));
              return round(dht->readTemperature());
         break;      
     case 76: //Evaluate humidity on device cmd->devid  
              devce=getDeviceById(cmd->devid);  
              dht=(DHT*)((void*)(devce->devpointer));
              return round(dht->readHumidity());              
          break;
     case 77: //Evaluate distance on device cmd->devid  
              devce=getDeviceById(cmd->devid);  
              sonic=(CP_SonicSensor*)((void*)(devce->devpointer));
              sonic->checkStatus();
              return sonic->distanceFront();              
          break;
     case 78://relP
             // devce=getDeviceById(cmd->devid);  
              //bmp=(SFE_BMP180*)((void*)(devce->devpointer)); no need only one device
              getPressure();
              return relP;
          break;    
     case 79://Pres
              getPressure();
              return Pres;
           break;   
     case 80://bmpTemp
              getPressure();
              return bmpTemp;
           break;
     case 81://Moisture or other analog read device
             devce=getDeviceById(cmd->devid);  
             return analogRead(getDevicePin(cmd->devid,1));
             break;
      
  }
}


boolean isIFfunc(int cmdid){                          
  switch(cmdid){
     
   case 94:  //check digitalpin is high
   case 95:   //check digitalpin is low
   case 136: //IR distance sensor 
   case 141: //Usonic Device if distance greater,etc
   case 146: //Switch device
   case 166: //Moisture
   case 171: //Temp GT,etc  
   case 173: //Humid GT,etc
   case 177: //pressure
   case 178: //relative pressure
   case 179: //temp from bmp180   
           return true;
   
   default:
           return false;    
  }

}


boolean evalfunc(DspCommand *cmd){
  
  boolean retval=true;  
  int checkparam;
  int paramtocheck;
  int opnd;
  int pin1;
  int v,p,d,t;
  actdevice* devce; 
  
 // DoDebug(F("testing:"),cmd->cmdid);
  checkparam=interpreter.getParam2(cmd);
  opnd=interpreter.getParam1(cmd);
  switch(cmd->cmdid){
     case 70:
     case 72:                                       
            // DoDebug("LOOP COMMAND",0);
             return true;
             break;
     case 94:      
          paramtocheck=digitalRead(checkparam);
          checkparam=HIGH;
          opnd=4; //equal
          break;     
     case 95:                     
          paramtocheck=digitalRead(checkparam);
          checkparam=LOW;
          opnd=4; //equal     
          break;     
     case 136: // //136 if IR distance>%p1
               devce=getDeviceById(cmd->devid);  
               pin1=getDevicePin(cmd->devid,1);
               paramtocheck=digitalRead(pin1);
               checkparam=LOW;
               opnd=4; //equal             
          break;          
     case 141: //distance>
               devce=getDeviceById(cmd->devid);  
               sonic=(CP_SonicSensor*)((void*)(devce->devpointer));
               sonic->checkStatus();
               paramtocheck=sonic->distanceFront();       
          break;
     case 146:  //check switch if pressed no params
                devce=getDeviceById(cmd->devid);  
                pin1=getDevicePin(cmd->devid,1);
               // DoDebug(F("Pin"),pin1);
                
                v=digitalRead(pin1);//get value of switch
                d=getDevicePin(cmd->devid,2); //Current value
               // p=getDevicePin(cmd->devid,3); //Prev value
                t=getDevicePin(cmd->devid,4); //time value
               // DoDebug(F("Read"),v);
               // DoDebug(F("Current"),d);
                if (v != d   && millis() - t > 100) {//debounce the switch for 200ms
                  d=v;//set new value we read
                  setDevicePin(cmd->devid,2,d); //Current value
                  setDevicePin(cmd->devid,4,millis()); //Time value
                }
                paramtocheck=d;
                opnd=4;//equal
                checkparam=HIGH; 
          break;
       case 166://eval if moisture > %p2 or other analog read device
                devce=getDeviceById(cmd->devid);  
                pin1=getDevicePin(cmd->devid,1);
                v=analogRead(pin1);
                paramtocheck=v;
                break;     
       case 171: //eval if temp>%p2   
                devce=getDeviceById(cmd->devid);
                dht=(DHT*)((void*)(devce->devpointer));
                t=round(dht->readTemperature());
             //   DoDebug(F("temp="),t);
             //   DoDebug(F("Param="),checkparam);
                paramtocheck=t;            
          break;
       case 173: //eval if humidity>%p2      
                devce=getDeviceById(cmd->devid);  
                dht=(DHT*)((void*)(devce->devpointer));
                t=round(dht->readHumidity());
             //   DoDebug(F("humid="),t);
             //   DoDebug(F("Param="),checkparam);
                paramtocheck=t;                               
          break;
        case 177://eval if press>%p2
                getPressure();
                paramtocheck=Pres;
                break;
        case 178://eval if relp>%p2
                getPressure();
                paramtocheck=relP;
                break;
        case 179://eval if bmptemp>%p2
                getPressure();
                paramtocheck=bmpTemp;
                break;
          
  }
 // DoDebug(F("PToCheck="),paramtocheck);
 // DoDebug(F("Param="),checkparam);
 // DoDebug(F("Check Opnd="),opnd);
  switch (opnd){
    case 0://IS
           retval=  paramtocheck==checkparam;
           break;
    case 1://Not is
           retval=  !(paramtocheck==checkparam);
           break;
    case 2://Greater
           retval=  paramtocheck>checkparam;
           break;
    case 3://Less
           retval=  paramtocheck<checkparam;
           break;
    case 4://Equal
           retval=  paramtocheck==checkparam;
           break;
    case 5://Greater than or equal
           retval=  paramtocheck>=checkparam;
           break;
    case 6://Less than or equal
           retval=  paramtocheck<=checkparam;
           break;
  }

 // DoDebug(F("Return="),retval);
  return retval;
  
}

boolean programExecute(){

  
  actdevice* devce;
  int com=-1;
  int d,n;
  float gp;
  int bd;
  String s;
  int pin1;

  
  //if (dspgyro->turning) return false; 
  if (!interpreter.running) {return false;}//DoDebug(F("prog END"),0);} 
  
  
  if (lastcommandexecuted){
     cmd=interpreter.getCommandToExecute();     
     if (cmd!=0){
       // DoDebug("Command:",cmd->cmdid); 
        com=cmd->cmdid;
        lastcommandexecuted=false;
        if (com!=lastcmdid)
         setupcmds=0;//new setup command
        else setupcmds++; 
     }  else  com=2;               
  }  
  else
     com=lastcmdid;

   if (com>-1)
     switch(com) {
       case 1:break;
       case 2:interpreter.endProgram();       
           //   DoDebug(F("PROGRAM TERMINATED!!!"),0);
            //  DoDebug(F("===================="),0);
              break;
//===============DESP Robot ============================ 
       case 50:   //Setup desp Robot
                if (setupcmds==0) {
                  setDevicePin(cmd->devid,1,cmd->param1);
                  setDevicePin(cmd->devid,2,cmd->param2);
                } else if (setupcmds==1) { 
                  setDevicePin(cmd->devid,3,cmd->param1);
                  setDevicePin(cmd->devid,4,cmd->param2);
                } else {       //3rd time we init the device                           
                  devce=getDeviceById(cmd->devid);
                  Robo=new CP_Robot;                
                  devce->devpointer=(int)(Robo);  
                  Robo->init(getDevicePin(cmd->devid,1),getDevicePin(cmd->devid,2),getDevicePin(cmd->devid,3),getDevicePin(cmd->devid,4),cmd->param1);
                  if (cmd->param1!=0){
                    pinMode (cmd->param1, INPUT) ;
                    attachInterrupt(digitalPinToInterrupt(cmd->param1), speedencint, CHANGE);
                  }
                }
              break;       
//===============DESP MOTOR ============================   
       case 60: //Setup despmotor
                setDevicePin(cmd->devid,1,cmd->param1);
                setDevicePin(cmd->devid,2,cmd->param2);
                devce=getDeviceById(cmd->devid);
                Motor=new DESP_DCMotor;                
                devce->devpointer=(int)(Motor);  
                Motor->init(++motid,cmd->param1,cmd->param2);
              break;      
       case 61: //Command motor
                n=interpreter.getParam1(cmd);//get motor command 1=forward 2=backward 4=stop
                devce=getDeviceById(cmd->devid);  
                Motor=(DESP_DCMotor*)((void*)(devce->devpointer));
                Motor->run(n);
              break;  
       //70,72 - special loop commands
       //75- 90 special fixed variables from devices
       //99  is a special command no use   END         
       //100 is a special command no use   ELSE    
       //--------- Devices setup & commands
//===============SPEAKER BUZZER ============================        
       case 110:       //setup speaker
                setDevicePin(cmd->devid,1,cmd->param1);//Speakerpin
                devce=getDeviceById(cmd->devid);  
                speak=new Speaker(cmd->param1);
                devce->devpointer=(int)(speak);    
              break;              
       case 111:
                devce=getDeviceById(cmd->devid);  
                speak=(Speaker*)((void*)(devce->devpointer));
                speak->Beep();
                break;              
       case 112:
                devce=getDeviceById(cmd->devid);  
                speak=(Speaker*)((void*)(devce->devpointer));
                n=interpreter.getParam1(cmd);//cmd->param1;
                d=interpreter.getParam2(cmd);//cmd->param2;
                speak->PlayTone(n,d);
                break;
//===============LCD SCREEN ============================                
       case 120:setuplcd(); //just one lcd so no need to keep track of the sda/scl address
              break;
       case 121:s=cmd->paramstr;
                LCDprint(s);
                //Serial.println(s);
                break;
       case 122:
                LCDclear();
                break;
       case 123:
                if (lcd!=NULL) 
                {
                  d=interpreter.getParam1(cmd);
                  n=interpreter.getParam2(cmd);
                //  DoDebug(F("LCD SET P1="),d);
                //  DoDebug(F("LCD SET P2="),n);
                  lcd->setCursor(d,n);
                }  
                break;
       case 124://LCD print number
                s=String(interpreter.getParam1(cmd)); 
                LCDprint(s);
                //Serial.println(s);
                break;
       case 125://LCD print STRING AND number
                s=cmd->paramstr;
                LCDprint(s);LCDprint(String(interpreter.getParam1(cmd)));
               // Serial.print(s);Serial.println(interpreter.getParam1(cmd));
                break;
//===============LASER============================                
       case 130: //Laser setup
                pinMode(cmd->param1,OUTPUT);
                setDevicePin(cmd->devid,1,cmd->param1);//no vars on setup
                break;     
       case 131: //Laser On
                pin1=getDevicePin(cmd->devid,1);
                digitalWrite(pin1, HIGH); 
                break;     
       case 132: //Laser Off
                pin1=getDevicePin(cmd->devid,1);
                digitalWrite(pin1, LOW); 
                break;
//===============IR SENSOR ============================                     
       case 135://IR Sensor Setup
                pinMode(cmd->param1,INPUT);
                setDevicePin(cmd->devid,1,cmd->param1);//no vars on setup commands
                break;     
           // //136 if IR distance==LOW on eval we have an obstacle

//===============ULTRASONIC Distance Sensor============================            
       case 140://UltraSonic Setup   
                setDevicePin(cmd->devid,1,cmd->param1);//trig
                setDevicePin(cmd->devid,2,cmd->param2);//echo 
                devce=getDeviceById(cmd->devid);  
                sonic=new CP_SonicSensor();
                devce->devpointer=(int)(sonic);
                
                sonic->setSonicParams(5,400,cmd->param1,cmd->param2);//min distance 5cm, check every 400ms           
                sonic->init();
               // DoDebug(F("US Setup ok"),0);delay(2000);
                break;
                //141 eval function if
                //142 eval function if   
//===============SWITCH various devices ============================                
       case 145://Switch Setup
                pinMode(cmd->param1,INPUT);
                setDevicePin(cmd->devid,1,cmd->param1);//no vars on setup
                setDevicePin(cmd->devid,2,LOW); //Current value
                //setDevicePin(cmd->devid,3,LOW); //Prev value
                setDevicePin(cmd->devid,4,0); //Time last check value
                break;     
            //146 if Switch==HIGH on eval switch is on
//===============MICROSERVO 90 ============================                
       case 150: //Servo setup
                devce=getDeviceById(cmd->devid);  
                serv=new Servo();
                serv->attach(cmd->param1);  //Connected Pin PWM capable      //no vars on setup                                                                                                                    
                devce->devpointer=(int)(serv);
                setDevicePin(cmd->devid,1,cmd->param1);
                setDevicePin(cmd->devid,2,90);//set current position as param2 not used
                break;     
       case 151: //Servo Left
                devce=getDeviceById(cmd->devid);  
                serv=(Servo*)((void*)(devce->devpointer));
                n=interpreter.getParam1(cmd);
                d=max(90-n,0);
                serv->write(d);
                setDevicePin(cmd->devid,2,d);//set current position as param2 not used
                break;     
       case 152: //Servo Right
                devce=getDeviceById(cmd->devid);  
                serv=(Servo*)((void*)(devce->devpointer));
                n=interpreter.getParam1(cmd);
                d=min(90+n,180);
                serv->write(d);
                setDevicePin(cmd->devid,2,d);//set current position as param2 not used
                break;         
       case 153: //Servo Left add
                devce=getDeviceById(cmd->devid);  
                serv=(Servo*)((void*)(devce->devpointer));
                d=getDevicePin(cmd->devid,2);//get current position as param2 not used
                n=interpreter.getParam1(cmd);
                d=max(d-n,0);
                serv->write(d);
                setDevicePin(cmd->devid,2,d);//set current position as param2 not used
                break;     
       case 154: //Servo Right add
                devce=getDeviceById(cmd->devid);  
                serv=(Servo*)((void*)(devce->devpointer));
                d=getDevicePin(cmd->devid,2);//get current position as param2 not used
                n=interpreter.getParam1(cmd);
                d=min(d+n,180);
                serv->write(d);
                setDevicePin(cmd->devid,2,d);//set current position as param2 not used
                break; 
//===============PHOTOINTERRUPT ============================                        
       case 160://Photointerrupt Setup for distance measure make a class for this
                //needs interrupt pin (2,3) for nano
                //needs count number for 1 cm so we can check distance i.e. 10 changes makes 1 cm
                break;
       case 161://clear distance counter
                break;
       //case 162://eval if distance traveled > %p2 in CM
       //case 163://eval if distance traveled < %p2 in CM
//===============MOISTURE SENSOR ============================
       case 165://Setup Moisture sensor analog pin device just read this or other analog read device
                pinMode(cmd->param1,INPUT);
                setDevicePin(cmd->devid,1,cmd->param1);//no vars on setup
                break;
             //81 moisture fix variable or other analog read device
       //case 166 //eval if moisture > %p2 or other analog read device
       //case 167 //eval if moisture < %p2 or other analog read device
//===============DHT TEMPERATURE ============================       
       case 170: //DHT temperature setup
               // DoDebug(F("Setup Temp"),cmd->param1);
                devce=getDeviceById(cmd->devid);  
                //DoDebug(F("DHT p1="),cmd->param1);
                //DoDebug(F("DHT p2="),cmd->param2);
                dht=new DHT(cmd->param1,cmd->param2,0);//pin, type  (#define DHT11 11,#define DHT22 22,#define DHT21 21,#define AM2301 21)
                devce->devpointer=(int)(dht);
                setDevicePin(cmd->devid,1,cmd->param1);//Pin  no vars on setup
                setDevicePin(cmd->devid,2,cmd->param2);//Device Type
                dht->begin();
            //    DoDebug(F("DHT Setup ok"),0);
               break;
       //case 171 //eval if temp>%p2
       //case 172 //eval if temp<%p2       
       //case 173 //eval if humidity>%p2
       //case 174 //eval if humidity<%p2   
//===============BMP180 BAROMETRIC SENSOR ============================           
       case 176://BMP180 barometric Setup ONLY ONE SDA/SCL Device
                devce=getDeviceById(cmd->devid);
                bmp=new SFE_BMP180;
                devce->devpointer=(int)(bmp);
                bmp->begin();
//                if (bmp->begin())
//                   Serial.println(F("BMP180 init success"));
//                else
//                    Serial.println(F("BMP180 init fail\n\n"));    
                break;  
                //78,79,80 are the device variables
       //case 177// eval if Pres>%p2           
       //case 178// eval if relP>%p2           
       //case 179// eval if bmpTemp>%p2

//===================SETUP PIN FOR OUTPUT===================================       
       case 200:          
                n=interpreter.getParam1(cmd);            
                pinMode(n, OUTPUT);
                break;
//===================SETUP PIN FOR INPUT===================================                       
       case 201:     
                n=interpreter.getParam1(cmd);                
                pinMode(n, INPUT);
                break;
//===================SET PIN TO HIGH===================================                       
       case 202:      
                n=interpreter.getParam1(cmd);              
                digitalWrite(n, HIGH);   
                break;   
//===================SET PIN TO LOW===================================                                       
       case 203:             
                n=interpreter.getParam1(cmd);         
                digitalWrite(n, LOW);
                break;  
//===================VARIABLES===================================                                       
       case 230://set variable to value
                //param2 has the idx of the var
                //param1 has the value
                n=interpreter.getParam1(cmd);         
                d=interpreter.getParam2(cmd);         
            //    DoDebug(F("Set param idx="),d);                
            //    DoDebug(F("Value="),n);
                pin1=interpreter.getVar(d);
            //    DoDebug(F("Old Value="),pin1);
                interpreter.setVar(d,n);
                pin1=interpreter.getVar(d);
            //    DoDebug(F("new Value="),pin1);                
                break;
       case 231://change variable by value
                //param2 has the idx of the var
                //param1 has the value
                n=interpreter.getParam1(cmd);         
                d=interpreter.getParam2(cmd);         
            //    DoDebug(F("Set param idx="),d);                
            //    DoDebug(F("Value="),n);
                pin1=interpreter.getVar(d);
            //    DoDebug(F("Old Value="),pin1);
                interpreter.setVar(d,pin1+n);//change the old value by n
                pin1=interpreter.getVar(d);
            //    DoDebug(F("new Value="),pin1);                
                break;
//===================DELAY MILLISECS===================================                                       
       case 250:
                n=interpreter.getParam1(cmd);
                delay(n);
                break;                               
      }        
      
  interpreter.commandExecuted();
 // DoDebug(F(" // Command Executed // "),0);
 // if (com==10 || com==20 || com==30 || com==40)                     
  lastcmdid=com;
  lastcommandexecuted=true;
  
  return !(com==10 || com==20 || com==30 || com==40);                    
}


void DoProgram()
{ boolean f;
  unsigned long tm;
  
  if (inprogram==true){
   // straightbearing=-1;
    lastcmdid=-1;
    lastcommandexecuted=true;    
    if (getProgramFromBT()){     
     inprogram=false;
     DoDebug(F("Execute Program"),0);
                                                                                            
     interpreter.startProgram();                                             
    }
  }
  else {
        //PROGRAM EXECUTING LOOP                                     
     //  tm=millis();
     //  do {
          f=programExecute();                    
          if (!interpreter.running) {
             PlatfMode=none;                    
             DoDebug(("Program Terminated!!!"),0);
          } 
      // } while (f and (millis()-tm<1000));                                                               
        
  }     
}

#define ALTITUDE 280.0 
void getPressure(){
  char status;
  double T,P,p0,a;
  

  if (bmp==NULL) exit;

  status = bmp->startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.

    status = bmp->getTemperature(T);
    if (status != 0)
    {
      // Print out the measurement:
    //  Serial.print("temperature: ");
    //  Serial.print(T,2);
    //  Serial.print(" deg C, ");
    //  Serial.print((9.0/5.0)*T+32.0,2);
    //  Serial.println(" deg F");

      bmpTemp=T;
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = bmp->startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = bmp->getPressure(P,T);
        if (status != 0)
        {
          // Print out the measurement:
         // Serial.print("absolute pressure: ");
         // Serial.print(P,2);
         // Serial.print(" mb, ");
         // Serial.print(P*0.0295333727,2);
         // Serial.println(" inHg");

          Pres=P;
          // The pressure sensor returns abolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sealevel function and your current altitude.
          // This number is commonly used in weather reports.
          // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
          // Result: p0 = sea-level compensated pressure in mb

          p0 = bmp->sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          //Serial.print("relative (sea-level) pressure: ");
          //Serial.print(p0,2);
          //Serial.print(" mb, ");
          //Serial.print(p0*0.0295333727,2);
          //Serial.println(" inHg");

          relP=p0;
          
          
          // On the other hand, if you want to determine your altitude from the pressure reading,
          // use the altitude function along with a baseline pressure (sea-level or other).
          // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
          // Result: a = altitude in m.

       //   a = bmp.altitude(P,p0);
        //  Serial.print("computed altitude: ");
         // Serial.print(a,0);
         // Serial.print(" meters, ");
         // Serial.print(a*3.28084,0);
         // Serial.println(" feet");}
        }
       // else Serial.println(F("error retrieving pressure\n"));
      }
     // else Serial.println(F("error starting pressure\n"));
    }
   // else Serial.println(F("error retrieving temperature\n"));
  }
 // else Serial.println(F("error starting temperature\n"));

  
}


void loop()
{
  if (prevmode!=PlatfMode){
     LCDclear();
     switch (PlatfMode) {
       case none:  delay(2000);
                   LCDprint(F("CP READY ..."));
                   break;
       case progload:  
          LCDprint(F("PROGRAMMING"));
         // delay(2000);
        //  smode="Prog";                    
          break;
  }
  }
  
  prevmode=PlatfMode;
  lastcmd='~';

  if (lastcmd=='~') lastcmd=BTread();
  if (int(lastcmd)==-1) lastcmd=Serial.read();


  if ((lastcmd!='~') && (int(lastcmd)!=-1)) {
                                
    Serial.println(int(lastcmd));
    LCDprint("CMD:"+lastcmd);

    switch (lastcmd) {
    case '1':
      inprogram=true;
      PlatfMode=progload;
      delay(500);
      break;
    case '0'://stop command
           // DoDebug(F("STOPPED!!!"),0);
            inprogram=false;
            PlatfMode=none;
            break;     
    }
  }

  
  switch (PlatfMode) {
  case none: 
            break;
    
  case progload: 
          smode="Prog";                    
          DoProgram();
          break;
  }
                            
 
}










