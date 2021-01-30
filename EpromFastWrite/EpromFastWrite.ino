

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
   if (b==0) digitalWrite(AddrPins[i],LOW);
    else digitalWrite(AddrPins[i],HIGH);    
  } 
}

void setData(unsigned int dat){
 int b;

  for (int i=0;i<8;i++){
   b=dat & (1 << i);
   if (b==0) digitalWrite(DataPins[i],LOW);
    else digitalWrite(DataPins[i],HIGH);    
  }  
}

byte getData(){
int b;

  b=0;
  for (int i=0;i<8;i++){
    if (digitalRead(DataPins[i])==HIGH)
      b|= (1 << i);
  }
  return b;
}

byte readByte(unsigned int addr){
  int b;
  
  setDataInput();
  setAddr(addr);
  digitalWrite(CE,LOW);
  digitalWrite(RD,LOW);
  b=getData();
  digitalWrite(RD,HIGH);
  return b;
}

void writeByte(unsigned int addr,byte b){
  setDataOutput();
  setAddr(addr);
  setData(b);
  digitalWrite(CE,LOW);
  digitalWrite(WR,LOW);
  digitalWrite(WR,HIGH);  
}


void doPermanent(){
const unsigned int Addresses[] = {0x0E38, 0x31C7, 0x03E0, 0x3C1F, 0x303F, 0x0FC0}; //byte is big enough

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

void doRecall(){
const unsigned int Addresses[] = {0x0E38, 0x31C7, 0x03E0, 0x3C1F, 0x303F, 0x0C63}; //byte is big enough

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

void doMultiWrite(unsigned int addr,unsigned int lensize){
byte b;
unsigned int i;
  
  setDataOutput();
  digitalWrite(CE,LOW);  
  i=0;
  do {
     if (Serial.available() > 0) {
      b=Serial.read();
      setData(b);
      setAddr(addr++);
      digitalWrite(WR,LOW);
      digitalWrite(WR,HIGH);      
      i++;
     }
  } while (i<lensize);
}

void doMultiRead(unsigned int addr, unsigned int lensize){
byte b;
unsigned int i;
  
  setDataInput();
  digitalWrite(CE,LOW);  
  for (i=0;i<lensize;i++){
      setAddr(addr++);
      digitalWrite(RD,LOW);
      b=getData();      
      digitalWrite(RD,HIGH);      
      Serial.write(b);
      i++;
     }
}

void showMenu(){
  Serial.println("w addr,byte");
  Serial.println("r addr");
  Serial.println("bw addr,no of bytes");
  Serial.println("br addr,no of bytes");
  Serial.println("p permanent store");
  Serial.println("c recall stored data");
}


void setup() {
  Serial.begin(115200);

  for (int i=0;i<8;i++){
    pinMode(AddrPins[i],OUTPUT);
  }    
  pinMode(WR,OUTPUT);
  pinMode(CE,OUTPUT);
  pinMode(RD,OUTPUT);

  Serial.print("Intialisation complete");  
  showMenu();
}

void loop() {
char c;
unsigned int addr,nobytes;
byte b;
String s,s1,s2;

  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
   c=Serial.read();
   switch (c){
    case 'w':
             s=Serial.readString();
             b=s.indexOf(',');
             s1=s.substring(0,b-1);
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
    case 'b':c=Serial.read();
             s=Serial.readString();
             b=s.indexOf(',');
             s1=s.substring(0,b-1);
             s2=s.substring(b+1);
             addr=s1.toInt();
             nobytes=s2.toInt();
             if (c=='w') 
              doMultiWrite(addr,nobytes);
             if (c=='r') 
              doMultiRead(addr,nobytes);              
             Serial.println("Multi Command Succesful"); 
             break;
    case 'p':doPermanent();
             Serial.println("EEprom Permanent Stored");
             break;
    case 'c':doRecall();
             Serial.println("EEprom Data Recalled");
             break;
    case 'm':showMenu();
             break;  
   }

  }
}
