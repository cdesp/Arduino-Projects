//---------------------------------------
//SN76489 Digital Complex Sound Generator
//---------------------------------------
#define D0 11
#define D1 10
#define D2 9
#define D3 8
#define D4 7
#define D5 6
#define D6 5
#define D7 4

//#define D0 4
//#define D1 5
//#define D2 6
//#define D3 7
//#define D4 8
//#define D5 9
//#define D6 10
//#define D7 11


#define WE 2
#define CE 13
//==============================================================
void setup()
{
  pinMode(D0, OUTPUT); pinMode(D1, OUTPUT); pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT); pinMode(D4, OUTPUT); pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT); pinMode(D7, OUTPUT);
  pinMode(WE, OUTPUT); digitalWrite(WE, HIGH);
  pinMode(CE, OUTPUT); digitalWrite(CE, HIGH);
  //------------------------------------------------------------
  SilenceAllChannels(); //mute all channels of SN76489
  delay(2000);

}
//==============================================================
void loop()
{ 
  SendByte(0x90); //channel 0 maximum volume
  //different tone frequencies @ channel 0
 // SendByte(0x8F); SendByte(0x3F); //122Hz tone
 // delay(4000);
  //--------------------------------------------------------
 // SendByte(0x81); SendByte(0x27); //200Hz tone
 // delay(4000);
  //--------------------------------------------------------
 // SendByte(0x81); SendByte(0x1A); //300Hz tone
 // delay(4000);
  //--------------------------------------------------------
 // SendByte(0x8A); SendByte(0x0F); //500Hz tone
 // delay(4000);
  //--------------------------------------------------------
  SendByte(0x83); SendByte(0x0F); //2500Hz tone ON 2.5mHZ
  delay(10000);
  //--------------------------------------------------------
  
  for(byte vol=0x91; vol<=0x9F; vol++) //attenuate gradually
  {
    SendByte(vol); delay(50);
  } 
}
//==============================================================
void PutByte(byte b)
{
  digitalWrite(D0, (b&1)?HIGH:LOW);
  digitalWrite(D1, (b&2)?HIGH:LOW);
  digitalWrite(D2, (b&4)?HIGH:LOW);
  digitalWrite(D3, (b&8)?HIGH:LOW);
  digitalWrite(D4, (b&16)?HIGH:LOW);
  digitalWrite(D5, (b&32)?HIGH:LOW);
  digitalWrite(D6, (b&64)?HIGH:LOW);
  digitalWrite(D7, (b&128)?HIGH:LOW);
}
//==============================================================
void SendByte(byte b)
{
  digitalWrite(CE, LOW);
  digitalWrite(WE, HIGH);
  PutByte(b);
  delay(1);
  digitalWrite(WE, LOW);
  delay(1);
  digitalWrite(WE, HIGH);
  digitalWrite(CE, HIGH);
}
//==============================================================
void SilenceAllChannels()
{
  SendByte(0x9F); //mute channel 0
  SendByte(0xBF); //mute channel 1
  SendByte(0xDF); //mute channel 2
  SendByte(0xFF); //mute noise channel
}
