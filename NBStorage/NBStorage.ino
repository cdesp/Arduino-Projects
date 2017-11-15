


#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // F Malpartida's NewLiquidCrystal library
#include <MemoryFree.h>
#include <SoftwareSerial.h>

#define butPlay A0
#define butREC  A1
#define butPrev A2
#define butNext A3
#define butStop 8
//free pins 2,7,9
#define butdel 200

//NBserial com
#define CTSpin 3 //White 
#define RTSpin 4 //red
#define RX 5     //blue
#define TX 6     //red

//LCD 16x2
#define I2C_ADDR    0x27  // Define I2C Address for controller

SoftwareSerial   NBSerial(RX, TX, false); // RX, TX
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

//SD Card
#define  cspin 10





char t[20];
int totalFiles=0;
int curfileID=0;
File curDir;
File curFile;
/*
void scanWire(){

  Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;

  Wire.begin();
  for (byte i = 1; i < 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
      {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1);  // maybe unneeded?
      } // end of good response
  } // end of for loop
  Serial.println ("Done.");
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");
}  // end of setup

*/



void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print(F("Initializing SD card..."));

  if (!SD.begin(cspin)) {
    Serial.println(F("failed!"));
    return;
  }
  Serial.println(F("done."));



  /*
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
  //  myFile.println("testing 1, 2, 3.");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  // re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("err open test.txt");
  }
  */


  /*
  myFile=SD.open("DESP");
   if (myFile) {
     myFile.rewindDirectory();
     printDirectory(myFile,0);
     myFile.close();
   }
    else Serial.println("err open root");*/

  //  scanWire();
  // Switch on the backlight
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print(F("LCD OK!!!")); delay(2000);


  openRoot();

  pinMode(CTSpin, INPUT_PULLUP);
  pinMode(RTSpin, OUTPUT);

  RTSON();
  NBSerial.begin(9600);

  pinMode(butPlay, INPUT);
  pinMode(butREC, INPUT);
  pinMode(butPrev, INPUT);
  pinMode(butNext, INPUT);
  pinMode(butStop, INPUT);  

  Serial.println(F("done!"));
}

boolean butStopPressed(){

  
  boolean res= digitalRead(butStop)==HIGH;
  do {} while (digitalRead(butStop)!=LOW); //wait for release
  if (res) {Serial.println(F("S"));delay(butdel);}
  return res;
}

boolean butNextPressed(){

  boolean res= digitalRead(butNext)==HIGH;
  do {} while (digitalRead(butNext)!=LOW); //wait for release
   if (res) {Serial.println(F("N"));delay(butdel);}
  return res;
}

boolean butPrevPressed(){

  boolean res= digitalRead(butPrev)==HIGH;
  do {} while (digitalRead(butPrev)!=LOW); //wait for release
   if (res) {Serial.println(F("P"));delay(butdel);}
  return res;
}

boolean butRECPressed(){

  boolean res= digitalRead(butREC)==HIGH;
  do {} while (digitalRead(butREC)!=LOW); //wait for release
  if (res) {Serial.println(F("R"));delay(butdel);}
  return res;
}


boolean butPlayPressed(){

  boolean res= digitalRead(butPlay)==HIGH;
  do {} while (digitalRead(butPlay)!=LOW); //wait for release
 if (res) {Serial.println(F("PL"));delay(butdel);}
  return res;
}


byte CTS() {
  return digitalRead(CTSpin);
}

void RTSON() {
  digitalWrite(RTSpin, HIGH);
}

void RTSOFF() {
  digitalWrite(RTSpin, LOW);
}

void lcdprint(const char* mes) {
  strncpy(t, mes, 17); 
  while (strlen(t)<14)
    strcat(t, "  ");
  lcd.print(t);
}

void lcdprintline0(const char* mes) {
   lcd.setCursor(0, 0);
  lcdprint(mes);
}

void lcdprintline1(const char *mes) {
  lcd.setCursor(0, 1);
  lcdprint(mes);
}

/*
void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      Serial.println("no files");
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
*/

boolean sendChar(char c) {
  boolean brk=false;
  
  RTSOFF();
  do { brk=butStopPressed(); } while ((CTS() == HIGH) and (not brk)); //if NB can read
  if (!brk)   
    NBSerial.write(c);  //send
  RTSON();

  return !brk;
}

boolean receiveChar(char &ch) {
 boolean got=false;
  
  RTSOFF();
  if (NBSerial.available()) {
    ch = NBSerial.read();
    got=true;
  }
  RTSON();
  return got;
}

void sendSelectedFile() {
  // re-open the file for reading:
  File myFile;
  char ts[20];
  int i = 0;
  byte b = 0;
  

  Serial.println(F("Sending:"));
  Serial.print(F("Directory:")); Serial.println(curDir.name());
  Serial.print(F("File:")); Serial.println(curFile.name());
  //myFile = SD.open(ts);
  myFile = curFile;
  myFile.seek(0);
  if (myFile) {
    Serial.println(F("File Opened for sending"));
    lcd.setCursor(12, 1);
    lcd.print(F("S"));
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      i++;
      if (i % 200 == 0) { // every 10 bytes
        lcd.setCursor(12, 1);
        b = !b;
        if (b)
          lcd.print(F("S"));
        else
          lcd.print(F(" "));

      }
      if (!sendChar(myFile.read())) break;
    }
    // close the file:
   // myFile.close();
    lcd.setCursor(12, 1);
    lcd.print(F(" "));
    Serial.println(F("File Sent ok"));
  } else {
    // if the file didn't open, print an error:
    Serial.println(F("error openning file"));
  }

}



void prevFile(){
  if (curfileID>1)  curfileID--;
  curFile=seekfile(curDir,curfileID);
}

void nextFile(){
 if (curfileID<totalFiles)  curfileID++;
 curFile=seekfile(curDir,curfileID);
}




void openRoot() {
  //lcdprintline1("ROOT DIR");
  //curDir = SD.open("/");
  openCurrentDirectory(curDir,true);
  printfile();
 // getTotalFiles(curDir);
 // curfileID=1;
 // curFile=seekfile(curDir,curfileID);
 // printfile();
}

void openCurrentDirectory(File dir,boolean goRoot){
 if (curDir) curDir.close();
 if (goRoot) dir = SD.open("/");
 curDir=dir; 
 curDir.rewindDirectory(); 
 Serial.print(F("Opening "));
 Serial.println(curDir.name());
 getTotalFiles(curDir);
 curfileID=1;
 curFile=seekfile(curDir,curfileID);
 printDirectory();
}

void getTotalFiles(File dir){
  totalFiles=0;
 while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      Serial.println("no files");
      break;
    }
    totalFiles++;
    entry.close();
 }  
 Serial.print("Total files:");Serial.println(totalFiles);
}

File seekfile(File dir,int pos){
 int cur=0;
 File entry;
 
 dir.rewindDirectory(); 
 
 if (curFile){
   if (strcmp(curFile.name(),curDir.name())!=0) {
    curFile.close();
    Serial.println(F("cur file closed"));
  }
 }
 while (true) {

    entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      Serial.println(F("no more files"));
      break;
    }
    cur++;
    if (cur==pos) break;
    entry.close();
 }  
 return entry; 
}


void createNewFile(){
 int i=1;
 char buf[3];         //the ASCII of the integer will be stored in this char array
 
  
  do{
    if (strcmp(curDir.name(),"/")!=0)
      strcpy(t,curDir.name());
    else strcpy(t,"");  
    strcat(t,"/");
    strcat(t,"Noname");
    itoa(i,buf,10); //(integer, yourBuffer, base)
    strcat(t,buf);
    strcat(t,".bas");
    Serial.println(t);
    i++;
  }
  while (SD.exists(t));
  if (curFile) curFile.close();
  curFile = SD.open(t, FILE_WRITE);
  printfile();
}

void NBRecord(){
  boolean brk=false;
  char ch;
  File temp;
  
  Serial.print(F("Record a file"));
  createNewFile();
  while (!brk){
     brk=butStopPressed();
      if (receiveChar(ch))
        curFile.write(ch);
  }
  curFile.close();
  Serial.print(F("File saved"));
  temp=curDir;
  curDir=curFile;
  curFile =temp;
  openCurrentDirectory(curFile,false);
}


void printfile() {


  lcd.setCursor(0, 0);
  if (totalFiles == 0) lcd.print(F("NO FILES"));
  else {
    lcd.print(curfileID);lcd.print(".");
    strcpy(t,curFile.name());
    if (curFile.isDirectory())
       strcat(t," (D)");
    lcdprint(t);
  }
}

void printDirectory(){
     if (strcmp(curDir.name(),"/")==0)
       strcpy(t, "ROOT DIR");
     else {
       strcpy(t, "DIR:"); strcat(t, curDir.name());
     }
     while (strlen(t)<14) {
        strcat(t," ");      
     }
     if (totalFiles<10) strcat(t,"0"); 
     lcdprintline1(t);
     lcd.print(totalFiles);
}

void loop() {
  //char ts[10];
  char ch = Serial.read();
  if (butNextPressed()) ch='w';
  else
  if (butPrevPressed()) ch='q';
  else
  if (butPlayPressed()) ch='o';
  else
  if (butRECPressed()) ch='r';
  else
  if (butStopPressed()) ch='x';

  
 
  switch (ch) {

    
    case 'q': prevFile(); //button 1
      printfile();
      break;
    case 'w': nextFile(); //button 2
      printfile();
      break;
    case 'o'://open dir or send file              //button 3 select or play
      if (curFile.isDirectory()){
        openCurrentDirectory(curFile,false);
        printDirectory();
        printfile();
      }
      else sendSelectedFile(); //send to NB

      break;
    case 'r': lcdprintline1("AGN TO RECORD?");delay(butdel);
              do {
                if (butRECPressed()) {lcdprintline1("RECORDING");NBRecord();printDirectory();
                        printfile();break;}
                if (butStopPressed()) {printDirectory(); break;}
                } while (true);
              

      break;
      case 'x': openRoot();
                break;
    /*
    case 'p':myFile=SD.open("DESP");
           //  myFile.rewindDirectory();
             if (!myFile) Serial.print("error");
             printDirectory(myFile,0);
             myFile.close();
             break;*/
    case 'f':  Serial.print(F("freeMemory()="));
      Serial.println(freeMemory());
      break;
    case 'g'://get a file from NB
      break;
    case 's': sendChar('A');
      Serial.println(F("char send"));
      break;
    case 't':RTSOFF();
      do {} while (not NBSerial.available());
      Serial.println("");
        receiveChar(ch);
      Serial.print(F("Received:"));
      Serial.print(byte(ch)); Serial.print(F("="));
      Serial.println(ch);
      break;
    case 'n':RTSON();
             break;
    case 'm':RTSOFF();
             break;   
  }

}

