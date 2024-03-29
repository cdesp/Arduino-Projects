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

//Arduino PINS all pins are crossover with the z80 equivelants i.e. Z80 CTS goes to Ard RTS
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
  SdFile file;  
};

const int MAXFILES=10;
strfile filelist[MAXFILES]; //Max 10 open files
SdFile curDir; //current directory
SdFile curFile;
int totalFiles;
char t[20];
//All commands 1 byte=CMD, 1 byte=ID, 2 bytes=Params  TOTAL 4 bytes
//for filename we issue a read string after the command
char cmdbuffer[5]; //incoming command buffer
char fname[30];
int Command,fid,param1,param2;

const int OPENCARD=1;
const int OPENFILE=2;
const int CLOSEFILE=3;
const int READBLOCK=6;
const int WRITEBLOCK=7;
const int POSITIONS=8;
const int POSITIONG=9;
const int LISTDIR=10;
const int CHANGEDIR=11;
const int FILESIZE=12;
const int ENDOFFILE= 13;
const int INVALIDCMD=99;

const int FCMDOK=200;
const int FNOTDIR=201;
const int FNOTFND=202;
const int FNOMOR=203;



//FOR rs232 COMMUNICATION
//Zilog RTS to this CTSpin ,Zilog CTS to this RTS pin
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
  
  RTSON(); //NOT READY TO RECEIVE
  //todo:set a timeout  
  do { } while (CTS() == HIGH); //if we can send
  mySerial.write(c);  //send
  //RTSOFF(); //READY TO RECEIVE

}

boolean receiveChar(char &ch) {
 boolean got=false;
  
  RTSOFF(); //READY TO RECEIVE
  if (mySerial.available()) {
    ch = mySerial.read();
   // Serial.print(ch);Serial.print("-");Serial.print(byte(ch));Serial.println(" ");
    got=true;
  }
  RTSON();//NOT READY TO RECEIVE
  return got;
}

//todo:set a timeout?
boolean DOreceiveChar(char &ch) { 
  while (!receiveChar(ch)) ;  
  return true;
}

void textReceive(char *fn,int ml){
  char ch;
  
  for (int i=0;i<ml;i++)
  {
     DOreceiveChar(ch);
     fn[i]=ch;
     if (ch==0) break;     
  }
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
   else Serial.println("OK");
  

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
    //  Serial.print(F(" <DIR>  "));
    }
    else{    
      sendtext((__FlashStringHelper *)F("        "));
    //  Serial.print((__FlashStringHelper *)F("        "));
    }
    entry.getName(fBuffer,35);sendtext(fBuffer);sendNL(); 
   // entry.printName(&Serial);Serial.println();
    cur++;
    entry.close();
  }
 sendNL();
 itoa(cur, fBuffer, 10);
 sendtext((__FlashStringHelper *)F("Total Files:"));sendtext(fBuffer);sendNL();
 sendChar(char(255));//signal directory list ends
 return 200;
}

byte doChangeDir(){
  SdFile fl;

  if (fl.open(fname, O_READ)){
      if (fl.isDir()){
        curDir.close();
        curDir=fl;
        Serial.println(F("Dir Change OK"));
        return FCMDOK; //all ok
      }    
      else {
        fl.close();
        return FNOTDIR; //dir name is not a dir
      }
  }
  else return FNOTFND;//dir name NOT FOUND
  
}

byte doOpenFile(){
 SdFile fl;
 char fullname[40];

 curDir.getName(fullname,40); 
 strcat(fullname,"/");
 strcat(fullname,fname);

 Serial.println(fullname);

 if (param1==0) {
    if (!fl.open(fullname, O_READ)) {
      Serial.println(F("File Not found"));
      return FNOTFND;
    }
    Serial.println(F("File open for read"));
 }
 if (param1==1) {//==Write file
     if (!fl.open(fullname, O_WRITE)) {
      Serial.println(F("File Not found"));
      return FNOTFND;
     }
     Serial.println(F("File open for write"));
 }
 if (param1==2) {//==Read/Write file
     if (!fl.open(fullname, O_RDWR)) {
      Serial.println(F("File Not found"));
      return FNOTFND;
     }
     Serial.println(F("File open for read/write"));
 }
 if (param1==4) {//==Create file
     if (!fl.open(fullname, O_CREAT | O_RDWR)) {
      Serial.println(F("File can not be created"));
      return FNOTFND;
     }
     Serial.println(F("File created and open for read/write"));
 }
  
  int idx=getemptyfileidx();
  if (idx==-1) {
    fl.close();
    return FNOMOR; //no more open files
  }
  filelist[idx].fid=idx+1;
  filelist[idx].file=fl;
  return idx;
}

byte doCloseFile(byte idx){

 if (filelist[idx].fid==0) return FCMDOK; 
 filelist[idx].fid=0;
 filelist[idx].file.close();
 return FCMDOK; 
}

byte doReadBlock(byte idx){
   SdFile fl;
  //read a block of bytes of a file
  //Z80 asks an amount ARD returns the bytes that will be sent <=Z80 request
  fl=filelist[idx].file;

  //Serial.println(fl.fileSize());
  //Serial.println(fl.curPosition());
 
  int z80ask=param1*256+param2; //2bytes 
  int bytesleft=fl.fileSize()-fl.curPosition();
  if ((z80ask==0) || (bytesleft<z80ask)) z80ask=bytesleft;  
  //send back the size we will send
  byte bh=z80ask/256;
  byte bl=z80ask%256;
  sendChar(bh);sendChar(bl);
  Serial.print(F("Bytes to return:"));Serial.println(z80ask);
  Serial.println(F("Returning bytes"));
  //send the block of data to z80
  for (int i=0;i<z80ask;i++){
    fl.read(&bl,1);  //read one byte
    sendChar(bl);
  }
  Serial.println(F("ok"));
 return FCMDOK; 
}

byte doWriteBlock(byte idx){
   SdFile fl;
   char ch;
   
   fl=filelist[idx].file;
   int btow=param1*256+param2;
   Serial.print(F("Bytes to write:"));Serial.println(btow);
   Serial.println(F("Writing bytes.."));   
   for (int i=0;i<btow;i++) {
    DOreceiveChar(ch);
    fl.write(ch);
   }
  Serial.println(F("ok"));   
  return FCMDOK;   
}


//RETURNS FILESIZE
byte doFilesize(byte idx){
   SdFile fl;

  fl=filelist[idx].file;

  int sz=fl.fileSize();
  byte bh=sz/256;
  byte bl=sz%256;
  sendChar(bh);sendChar(bl);
  return FCMDOK; 
}

//SETS THE POSITION
byte doSetPosition(byte idx){
   SdFile fl;
   
  fl=filelist[idx].file;
  
  int newpos=param1*256+param2;
  fl.seekSet(newpos); //from the start of file
  return FCMDOK; 
}

//RETURNS THE POSITION
byte doGetPosition(byte idx){
   SdFile fl;
   
  fl=filelist[idx].file;
  
  int pos=fl.curPosition();
  byte bh=pos/256;
  byte bl=pos%256;
  sendChar(bh);sendChar(bl);
  return FCMDOK; 
}


byte doGetEOF(byte idx){
   SdFile fl;
   
  fl=filelist[idx].file;
  if (fl.peek()==-1) return 255;
   else return FCMDOK;  
}

void executeCommand(){

  byte retparam=255;
  Serial.print(F("command:"));Serial.println(Command);
  switch (Command){
    case OPENCARD:  openRoot();                    
                    retparam=totalFiles; //no more than 255 files i hope
                    break;
    case OPENFILE:  retparam=doOpenFile();
                    break;
    case CLOSEFILE: retparam=doCloseFile(fid);
                    break;
    case READBLOCK: retparam=doReadBlock(fid);
                    break;
    case WRITEBLOCK: break;
    case POSITIONS: retparam=doSetPosition(fid);
                    break;
    case POSITIONG:  retparam=doGetPosition(fid);
                    break;                    
    case LISTDIR:   retparam=doListDir();//todo:check if we have a name with the command and list that dir if exists
                    break;
    case CHANGEDIR: retparam=doChangeDir();
                    break;  
    case FILESIZE:  retparam=doFilesize(fid);
                    break;
    case ENDOFFILE: retparam=doGetEOF(fid);  
                    break;                 
  }

  sendChar(char(retparam));//always return a command result to Z80
  Serial.print(F("Command Completed:"));Serial.println(byte(retparam));
}



void interconnectports(){
 if (mySerial.available())
    Serial.write(mySerial.read());
  if (Serial.available())
    mySerial.write(Serial.read());   
}

void loop1(void) {
  char ch;
    RTSON();
    if (receiveChar(ch)){
     Serial.print(ch);Serial.print("-");Serial.print(byte(ch));Serial.println(" ");
    }
  }

void loop2(void){
char ch;
int i=0;
Serial.print('[');
  while (i<10){
    if (receiveChar(ch)){
      Serial.print(ch);
      i++;
     //Serial.print(ch);Serial.print("-");Serial.print(byte(ch));Serial.println(" ");
    }      
  }
Serial.println(']');  
}

void loop3(void){
    sendtext(F("1234567890"));
    Serial.println(F("Send ok"));    
}

void loop5(){
  if (Serial.available()){
    char ch=Serial.read();
    if (ch=='S')
      loop3();
    if (ch=='R')
      loop2();  
    Serial.println("S,R");  
  }
}

void loop99(void) {
  loop2();
}

void loop(void) {
  char ch;
 
  mySerial.flush();
  do {
    DOreceiveChar(ch);
    
  } while (ch==0);
  Serial.print(F("Getting command ["));
  cmdbuffer[0]=ch;
  Serial.print(byte(ch));Serial.print(" ");
    //take command 4 bytes
   
    for(int i=1;i<4;i++) {
     //char ch=mySerial.read();
     DOreceiveChar(ch);
     cmdbuffer[i]=ch;
     Serial.print(byte(ch));Serial.print(" ");
    } 
    Serial.println("]");
   Command=cmdbuffer[0];
   fid=cmdbuffer[1];
   param1=cmdbuffer[2];
   param2=cmdbuffer[3];
 
   if (Command==OPENFILE || Command==CHANGEDIR) {
    Serial.print(F("GETTING FILENAME:"));
    textReceive(fname,30);
    Serial.println(fname);
   }
    //mySerial.readString().toCharArray(fname,30);
   executeCommand();        
}
