/*
  SD card test

  This example shows how use the utility libraries on which the'
  SD library is based in order to get info about your SD card.
  Very useful for testing a card when you're not sure whether its working or not.

  The circuit:
    SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
 ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
 ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
 ** CS - depends on your SD card shield or module.
     Pin 4 used here for consistency with other Arduino examples


  created  28 Mar 2011
  by Limor Fried
  modified 9 Apr 2012
  by Tom Igoe
*/
// include the SD library:
#include <SPI.h>
#include "SdFat.h"
#include <SoftwareSerial.h>

#define CTSpin 4 //White 
#define RTSpin 5 //red
#define RX 2     //blue
#define TX 3     //red

SoftwareSerial mySerial(RX, TX); // RX, TX  d2,d3



// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// MKRZero SD: SDCARD_SS_PIN
const int chipSelect = 10;
SdFat SD;

struct strfile {
  byte fid;
  SdFile fobj;  
};

const int MAXFILES=10;
strfile filelist[MAXFILES]; //Max 10 open files
SdFile curDir; //current directory
SdFile curFile;
int totalFiles;
char t[20];

//FOR rs232 COMMUNICATION
byte CTS() {
  return digitalRead(CTSpin);
}

void RTSON() {
  digitalWrite(RTSpin, HIGH);
}

void RTSOFF() {
  digitalWrite(RTSpin, LOW);
}



void sendChar(char c) {
  boolean brk=false;
  
  RTSOFF();
  //todo:set a timeout  
  do { } while (CTS() == HIGH); //if we can send
  mySerial.write(c);  //send
  RTSON();

}

boolean receiveChar(char &ch) {
 boolean got=false;
  
  RTSOFF();
  if (mySerial.available()) {
    ch = mySerial.read();
    got=true;
  }
  RTSON();
  return got;
}

void sendtext(const __FlashStringHelper* tx){
 byte c;
   
  for (int i=0;i<strlen_P(reinterpret_cast<const char *>(tx));i++){
    c=pgm_read_byte_near(reinterpret_cast<const char *>(tx)+i);
    if (c==0) break;
    sendChar(char(c));
  } 
}

void sendtext(const char tx[]){

  for (int i=0;i<strlen(tx);i++){
   // Serial.print("Send.. ");Serial.println(tx[i]);
    if (byte(tx[i])==0) break;
    sendChar(tx[i]);
  }
}

void sendNL(){
  sendChar(char(13));
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  pinMode(CTSpin, INPUT_PULLUP);
  pinMode(RTSpin, OUTPUT);

  Serial.print("\nInitializing SD card...");

  // Initialize SdFat or print a detailed error message and halt
  // Use half speed like the native library.
  // change to SPI_FULL_SPEED for more performance.
  if (!SD.begin(chipSelect, SPI_HALF_SPEED)) SD.initErrorHalt();
  

 // set the data rate for the SoftwareSerial port
  mySerial.begin(38400);
  
  openRoot();
 // doListDir(curDir);
}

void openRoot() {
  if (curDir) curDir.close();
  curDir.open("/");
}

void openCurrentDirectory(SdFile dir,boolean goRoot){
 if (curDir) curDir.close();
 if (goRoot) dir.open("/");
 curDir=dir; 
 curDir.rewind(); 
 Serial.print(F("Opening "));
 curDir.printName(&Serial);Serial.println();
 getTotalFiles(curDir);
}

void getTotalFiles(SdFile dir){
  SdFile fl;
  totalFiles=0;
   // open next file in root.  The volume working directory, vwd, is root
  while (fl.openNext(&dir, O_RDONLY)) {
    //fl.printName(&Serial);Serial.println();
    fl.close();
    totalFiles++;
  }

 Serial.print("Total files:");Serial.println(totalFiles);
}


int getemptyfileidx(){
  for (int i=0;i<MAXFILES-1;i++){
    strfile sf=filelist[i];
    if (sf.fid==0) return i;
  }
  return -1;
}


//All commands 1 byte=CMD, 1 byte=ID, 2 bytes=Params  TOTAL 6 bytes
//for filename we issue a read string after the command

//opencard cmd= OCD  [refresh sd card]
//openfile cmd= OFL id,"fname" 
//closefile cmd= CFL id
//readbyte cmd= RBT id [ret byte]
//writebyte cmd= WBT id,byte
//readblock cmd= RBL id,noofbytes [loop to send them back]
//writeblock cmd= WBL id,noofbytes [loop to read an write them to sd]
//position  cmd= PFL id [ret 2 bytes with position in file]
//seek cmd = SFL id,newpos [2 bytes]

char cmdbuffer[5]; //incoming command buffer
char fname[30];
int Command,fid,param1,param2;

const int OPENCARD=1;
const int OPENFILE=2;
const int CLOSEFILE=3;
const int READBYTE=4;
const int WRITEBYTE=5;
const int READBLOCK=6;
const int WRITEBLOCK=7;
const int POSITION=8;
const int SEEKFILE=9;
const int LISTDIR=10;
const int CHANGEDIR=11;
const int INVALIDCMD=99;

const int dely=100;

byte doListDir(){
 int cur=0;
 SdFile entry;
 char fBuffer[40];
 
 sendNL();
 curDir.getName(fBuffer,35); 
 sendtext(F("DIRECTORY LISTING ["));sendtext(fBuffer);sendtext((__FlashStringHelper *)F("]"));sendNL();
  curDir.rewind();
  sendNL();Serial.println();
  while (entry.openNext(&curDir, O_READ)) {
    if (entry.isDir()) {
      sendtext((__FlashStringHelper *)F(" <DIR>  "));
      Serial.print(F(" <DIR>  "));
    }
    else{    
      sendtext((__FlashStringHelper *)F("        "));
      Serial.print((__FlashStringHelper *)F("        "));
    }
    entry.getName(fBuffer,35);sendtext(fBuffer);sendNL(); 
    entry.printName(&Serial);Serial.println();
    cur++;
    entry.close();
  }
 sendNL();
 itoa(cur, fBuffer, 10);
 sendtext((__FlashStringHelper *)F("Total Files:"));sendtext(fBuffer);sendNL();
 sendChar(char(255));//signal directory list ends
 return 0;
}

byte doChangeDir(){
  char fnm[30];
  SdFile fl;
  
  //Serial.print("ChgDir:");Serial.println(fname);
  //curDir.printName(&Serial);
  curDir.rewind();
  while (fl.openNext(&curDir, O_RDONLY)) {    
    fl.getName(fnm, 30);
    //Serial.print("[");Serial.print(fnm);Serial.print("]-[");Serial.print(fname);Serial.println("]");
    //Serial.println(strcmp(fname,fnm));
    if (strcmp(fname,fnm)==0){
      if (fl.isDir()){
        curDir.close();
        curDir=fl;
        Serial.println(F("Dir Change OK"));
        return 0; //all ok
      }
      else return 1; //dir name is not a dir
    }
    fl.close();   
  }

   return 2; //dir name not found
  
}

void executeCommand(){
  byte flid=cmdbuffer[3];
  byte param1=cmdbuffer[4];
  byte param2=cmdbuffer[5];
  byte retparam=0;
  Serial.print(F("command:"));Serial.println(Command);
  switch (Command){
    case OPENCARD:  openRoot();                    
                    retparam=totalFiles; //no more than 255 files i hope
                    break;
    case OPENFILE:  break;
    case CLOSEFILE: break;
    case READBYTE: break;
    case WRITEBYTE: break;
    case READBLOCK: break;
    case WRITEBLOCK: break;
    case POSITION: break;
    case SEEKFILE: break;
    case LISTDIR:   retparam=doListDir();//todo:check if we have a name with the command and list that dir if exists
                    break;
    case CHANGEDIR: retparam=doChangeDir();
                    break;    
  }

  sendChar(char(retparam));//always return a command result to Z80
  Serial.print(F("Command Completed"));
}



void interconnectports(){
 if (mySerial.available())
    Serial.write(mySerial.read());
  if (Serial.available())
    mySerial.write(Serial.read());   
}

void loop(void) {
  if (mySerial.available()){
    //take command 4 bytes
    Serial.println(F("Getting command"));
    for(int i=0;i<4;i++) {
     char ch=mySerial.read();
     cmdbuffer[i]=ch;
    }
   Command=cmdbuffer[0];
   fid=cmdbuffer[1];
   param1=cmdbuffer[2];
   param2=cmdbuffer[3];
 
   if (Command==OPENFILE || Command==CHANGEDIR)
    mySerial.readString().toCharArray(fname,30);
   executeCommand();      
  }
}
