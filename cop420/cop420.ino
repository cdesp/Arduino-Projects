#include "digitalWriteFast.h"


void setCOPDataOUT(){
  pinModeFast(10,OUTPUT);  //L0
  pinModeFast(3,OUTPUT);
  pinModeFast(4,OUTPUT);
  pinModeFast(5,OUTPUT);
  pinModeFast(6,OUTPUT);
  pinModeFast(7,OUTPUT);
  pinModeFast(8,OUTPUT); //
  pinModeFast(9,OUTPUT); //L7
}

void setCOPDataIN(){
  pinModeFast(10,INPUT);  //L0
  pinModeFast(3,INPUT);
  pinModeFast(4,INPUT);
  pinModeFast(5,INPUT);
  pinModeFast(6,INPUT);
  pinModeFast(7,INPUT);
  pinModeFast(8,INPUT); // 
  pinModeFast(9,INPUT); //L7 
}


void setCOPData(byte b){
 digitalWriteFast(10,b & 1); 
 digitalWriteFast(3,b & 2); 
 digitalWriteFast(4,b & 4); 
 digitalWriteFast(5,b & 8); 
 digitalWriteFast(6,b & 16); 
 digitalWriteFast(7,b & 32); 
 digitalWriteFast(8,b & 64); 
 digitalWriteFast(9,b & 128);  
 delayMicroseconds(20);
// delay(100);
}

byte getCOPData(){
 byte b;
  
  b=digitalReadFast(10);
  b=b+digitalReadFast(3)*2;
  b=b+digitalReadFast(4)*4;
  b=b+digitalReadFast(5)*8;
  b=b+digitalReadFast(6)*16;
  b=b+digitalReadFast(7)*32;
  b=b+digitalReadFast(8)*64;
  b=b+digitalReadFast(9)*128;

  return b;
}

int SI= A4;
int SO= 12;
int RES=A5;
int CLK=A0;



void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);   
  setCOPDataOUT();

  pinMode(CLK,OUTPUT); //clock to cop
  pinMode(SO,OUTPUT);  //SO control no more than 3 volts
  pinMode(SI,OUTPUT);  //SI 
  pinMode(RES,OUTPUT);  //reset when 0
  pinMode(11,INPUT);
  pinMode(2,INPUT);
  digitalWrite(RES,HIGH);
  digitalWrite(SI,LOW);  
  digitalWrite(SO,LOW);  

  attachInterrupt(digitalPinToInterrupt(2), getbyte, RISING );//WAS RISING
  
  Serial.println("Setup OK");
 // analogWrite(CLK,1);
}

void clkCOP(){
  
  digitalWriteFast(CLK,HIGH);
  delayMicroseconds(50);
  digitalWriteFast(CLK,LOW);  
  delayMicroseconds(50);
 
  
}

void clkCycle(int n){

  //n*=8;
  for (int i=0;i<n;i++){ //8 times the internal cop clock
    clkCOP();    
  }
}

void EnterCopTestMode(){
  int c=4;
  setCOPDataOUT();
  delayMicroseconds(50);
  setCOPData(0); //zero out
  Serial.println("Entering Cop Test DUMP");
  digitalWrite(RES,LOW);  //reset cop low
//  digitalWrite(SI,LOW);  //Si instruction mode
//  digitalWrite(SO,LOW);  //SO to 2.65volts high
  delayMicroseconds(100);
  clkCycle(c);
  digitalWrite(SO,HIGH);  //SO to 2.65volts high
  clkCycle(c);
  digitalWrite(SI,HIGH);  //Si instruction mode
  clkCycle(c);
 // Serial.println("SO HIGH");
//   delay(5000);
  digitalWrite(RES,HIGH);  //reset cop high start
  delayMicroseconds(6);
//  delay(100);
 // Serial.println("Res HI");
//  delay(5000);
 
  
  setCOPData(0); //CLRA                   $00
  waitForState();
  setCOPData(0x63); // JMP 3FC (ROM SIZE)    $63 $fc  
  waitForState();
  setCOPData(0xfc); // JMP 3FC (ROM SIZE)    $63 $fc
  waitForState(); 
  setCOPData(0xbf); // LQID                  $bf
  waitForState();
  setCOPData(0x44); // O44H                  $44  
  waitForState(); ;
  setCOPData(0x33); // LEI 4                $33 $44
  waitForState();
  setCOPData(0x64); // LEI 4                $33 $44
  waitForState(); 
  setCOPData(0x44); // NOP                   $46
  waitForState(); 
 // setCOPData(0);

  
//  Serial.println("Si going LOW");
 // delay(1000);
  setCOPDataIN();
 // delayMicroseconds(1);
  digitalWrite(SI,LOW);  //Si DUMP MODE
//  Serial.println("Si LOW");
//  delay(2000);
 
}

void runCOP(){
  digitalWrite(SO,LOW);  //SO to 2.65volts high
  digitalWrite(RES,LOW);  //reset cop low
  clkCycle(20);
  digitalWrite(RES,HIGH);  //reset cop high start
  clkCycle(20);
  clkCycle(20);clkCycle(20);clkCycle(20);
}

volatile byte b;
volatile int state = LOW;
byte myInts[1200];

void getbyte(){
  
  state=HIGH;
}

void waitForState(){
  state=LOW;
  do{
    clkCycle(1);}
  while (state==LOW);  
  delayMicroseconds(1); 
}

void loop() {
  int i;
  // put your main code here, to run repeatedly:
 

 
  Serial.println("WAIT");
  delay(1000);
  
  EnterCopTestMode();
  
 //  runCOP();
   state=LOW;

  for ( i=0;i<1050;i++){
   
   //b=getCOPData();
    while (state == LOW){
    clkCycle(1); 
    }    
    b=getCOPData();
    myInts[i]=b;
    
    state=LOW;
  }

   for ( i=0;i<1023;i++){
    // Serial.print(i);Serial.print(":");
     Serial.println(myInts[i]);     
    }

   
  Serial.println("END");
  for (int k=0;k<1;){
   // Serial.println(digitalRead(10));     
   waitForState(); 
   //clkCycle(1);
   //b=getCOPData();
   //Serial.println(b);     
  }
 
}
