

const byte AddrPins[] = {53,51,49,47,45,43,41,39,37,35,33,31,29,27,25,23 }; //byte is big enough
const byte DataPins[] = {38, 40, 42, 44, 46, 48, 50, 52}; //byte is big enough
const byte WR= 7;
const byte CE= 6;
const byte RD= 5;

void setDataInput(){
  for (int i=0;i<8;i++){
    pinMode(DataPins[i],INPUT);
  }
}

void setDataOutput(){
  for (int i=0;i<8;i++){
    pinMode(DataPins[i],OUTPUT);
  }  
}

void setAddr(unsigned int ad){
 unsigned int b;

  for (int i=0;i<16;i++){
   b=ad & (1 << i);    
  // Serial.print("ab=");Serial.println(b);
   if (b==0) digitalWrite(AddrPins[i],LOW);
    else digitalWrite(AddrPins[i],HIGH);    
  } 
  delay(1);
}

void setData(byte dat){
 byte b;

  for (int i=0;i<8;i++){
   b=dat & (1 << i);   
  // Serial.print("pin=");Serial.print(DataPins[i]);Serial.print("=");Serial.println(b);
   if (b==0) digitalWrite(DataPins[i],LOW);
    else digitalWrite(DataPins[i],HIGH);    
  }  
  delay(1);
}

byte getData(){
byte b;

  b=0;
  for (int i=0;i<8;i++){
    if (digitalRead(DataPins[i])==HIGH)
      b|= (1 << i);
      //Serial.print("db=");Serial.println(b);
  }
  return b;
}

byte readByte(unsigned int addr){
  byte b;

  setAddr(addr);
  digitalWrite(WR,HIGH);  
  setDataInput();
  digitalWrite(CE,LOW);        
  digitalWrite(RD,LOW);  
  b=getData();
  digitalWrite(RD,HIGH);
  return b;
}

void doMultiRead(unsigned int addr, unsigned int lensize){
byte b;
unsigned int i;

  //Serial.println(addr);    
  //Serial.println(lensize);    
  setAddr(addr);
  digitalWrite(WR,HIGH);   
  setDataInput();
  digitalWrite(CE,LOW);  
  digitalWrite(RD,LOW);
  for (i=0;i<lensize;i++){      
      
      b=getData();     
      b=getData(); 
      b=getData(); 
      b=getData(); 
      b=getData(); 
      //digitalWrite(RD,HIGH);      
      Serial.write(b);
      addr++;
      setAddr(addr);
     }
}

void writeByte(unsigned int addr,byte b){

  digitalWrite(RD,HIGH);
  setDataOutput();
  setAddr(addr);
  setData(b);  
  digitalWrite(CE,LOW);
  digitalWrite(WR,LOW);
  digitalWrite(WR,HIGH);  
}

void doMultiWrite(unsigned int addr,unsigned int lensize){
byte b;
unsigned int i;
  
   digitalWrite(RD,HIGH);
     setDataOutput();
   digitalWrite(CE,LOW);
     
  
  i=0;
  do {    
     setAddr(addr);
     if (Serial.available() > 0) {
      b=Serial.read();
      //writeByte(addr,b);
      if (i<1) delay(1000);
      Serial.write(b); //send back
      setData(b);     
      digitalWrite(WR,LOW);
      digitalWrite(WR,HIGH); 
      digitalWrite(WR,LOW);
      digitalWrite(WR,HIGH); 
      
      //digitalWrite(CE,HIGH);   
    // digitalWrite(CE,HIGH);
   //digitalWrite(WR,HIGH);
   //digitalWrite(RD,LOW);              
      i++;addr++;
     }
  } while (i<lensize);
}

void doPermanent(){
const unsigned int Addresses[] = {0x0E38, 0x31C7, 0x03E0, 0x3C1F, 0x303F, 0x0FC0}; 

  setDataInput();
  digitalWrite(CE,HIGH);
  digitalWrite(RD,LOW);  
  digitalWrite(WR,HIGH);
  for (int i=0;i<6;i++){
    setAddr(Addresses[i]);
    digitalWrite(CE,LOW);
    delay(1);
    digitalWrite(CE,HIGH);
  }
  digitalWrite(RD,HIGH);
}

void doRecall(){
const unsigned int Addresses[] = {0x0E38, 0x31C7, 0x03E0, 0x3C1F, 0x303F, 0x0C63}; 

  setDataInput();
  digitalWrite(RD,LOW);
  digitalWrite(CE,HIGH);
  digitalWrite(WR,HIGH);
  for (int i=0;i<6;i++){
    setAddr(Addresses[i]);
    digitalWrite(CE,LOW);
    digitalWrite(CE,HIGH);
  }
  digitalWrite(RD,HIGH);
}


void showMenu(){
  Serial.println(" ");
  Serial.println("========================================");
  Serial.println("r:read byte (r addr)");
  Serial.println("w:write byte (w addr,byte)");
  Serial.println("br:binary read bytes (br addr,length)");
  Serial.println("bw:binary write bytes (bw addr,length)");
  Serial.println("p:permanent save bytes");
  Serial.println("z:recall bytes");
}


void setup() {
  Serial.begin(115200);

  for (int i=0;i<16;i++){
    pinMode(AddrPins[i],OUTPUT);
  }    
  pinMode(WR,OUTPUT);
  pinMode(CE,OUTPUT);
  pinMode(RD,OUTPUT);

  Serial.println("Initialisation complete");  
  showMenu();
}

void loop() {
char c;
unsigned int addr,nobytes;
byte b;
String s,s1,s2;

  digitalWrite(CE,HIGH);
   digitalWrite(WR,HIGH);
   digitalWrite(RD,LOW);
  if (Serial.available() > 0) {
   c=Serial.read();
   switch (c){
    case 'w':
             s=Serial.readString();
             b=s.indexOf(',');
             s1=s.substring(0,b);
             s2=s.substring(b+1);
             addr=s1.toInt();
             b=s2.toInt();
             writeByte(addr,b);
             Serial.print(addr);Serial.print(" <-- ");Serial.println(b);
             break;
    case 'r':
             s=Serial.readString();
             addr=s.toInt();
             b=readByte(addr);
             Serial.print(addr);Serial.print(" --> ");Serial.println(b);
             break;
    case 'b':s=Serial.readString();
             c=s.charAt(0);
             s=s.substring(2);
             b=s.indexOf(',');
             s1=s.substring(0,b);
             s2=s.substring(b+1);
             addr=s1.toInt();
             nobytes=s2.toInt();         
             if (c=='w') 
              doMultiWrite(addr,nobytes);
             if (c=='r') 
              doMultiRead(addr,nobytes);              
             Serial.println("Multi Command Succesfull"); 
             break;
    case 'p':doPermanent();
             Serial.println("EEprom Permanent Stored");
             break;
    case 'z':doRecall();
             Serial.println("EEprom Data Recalled");
             break;
    case 'm':showMenu();
             break;  
   }

  
  }
}
