/***************************************************
//Web: http://www.buydisplay.com
EastRising Technology Co.,LTD
Examples for ER-TFTM070-4V2.1(800x480 Pixels) with Capacitive Touch Panel
I2C Interface for Capacitive Touch Panel,8080 16-bit Interface for Display,5V Power Supply
This program is a demo of how to use capacitive touch panel

Tested and worked with:
Arduino Mega 2560,Arduino Due
Works with Arduino 1.6.0 IDE 
****************************************************/

#include <stdint.h>
#include <UTFT.h>
#include <SPI.h>
#include <Wire.h>


uint8_t addr  = 0x38;  //CTP IIC ADDRESS
// Declare which fonts we will be using
extern uint8_t SmallFont[];

// Set the pins to the correct ones for your development shield
// ------------------------------------------------------------
// Standard Arduino Mega/Due shield            : <display model>,38,39,40,41

#define FT5206_WAKE 11
#define FT5206_INT   48    
// Remember to change the model parameter to suit your display module!
UTFT myGLCD(SSD1963_800480,38,39,40,41);  //(byte model, int RS, int WR, int CS, int RST, int SER)
uint16_t tx, ty;

enum {
  eNORMAL = 0,
  eTEST   = 0x04,
  eSYSTEM = 0x01
};

struct TouchLocation
{
  uint16_t x;
  uint16_t y;
};

TouchLocation touchLocations[5];

uint8_t readFT5206TouchRegister( uint8_t reg );
uint8_t readFT5206TouchLocation( TouchLocation * pLoc, uint8_t num );
uint8_t readFT5206TouchAddr( uint8_t regAddr, uint8_t * pBuf, uint8_t len );

uint32_t dist(const TouchLocation & loc);
uint32_t dist(const TouchLocation & loc1, const TouchLocation & loc2);

bool sameLoc( const TouchLocation & loc, const TouchLocation & loc2 );


uint8_t buf[30];

uint8_t readFT5206TouchRegister( uint8_t reg )
{
  Wire.beginTransmission(addr);
  Wire.write( reg );  // register 0
  uint8_t retVal = Wire.endTransmission();
  
  uint8_t returned = Wire.requestFrom(addr, uint8_t(1) );    // request 6 uint8_ts from slave device #2
  
  if (Wire.available())
  {
    retVal = Wire.read();
  }
  
  return retVal;
}

uint8_t readFT5206TouchAddr( uint8_t regAddr, uint8_t * pBuf, uint8_t len )
{
  Wire.beginTransmission(addr);
  Wire.write( regAddr );  // register 0
  uint8_t retVal = Wire.endTransmission();
  
  uint8_t returned = Wire.requestFrom(addr, len);    // request 1 bytes from slave device #2
  
  uint8_t i;
  for (i = 0; (i < len) && Wire.available(); i++)
  {
    pBuf[i] = Wire.read();
  }
  
  return i;
}

uint8_t readFT5206TouchLocation( TouchLocation * pLoc, uint8_t num )
{
  uint8_t retVal = 0;
  uint8_t i;
  uint8_t k;
  
  do
  {
    if (!pLoc) break; // must have a buffer
    if (!num)  break; // must be able to take at least one
    
    uint8_t status = readFT5206TouchRegister(2);
    
    static uint8_t tbuf[40];
    
    if ((status & 0x0f) == 0) break; // no points detected
    
    uint8_t hitPoints = status & 0x0f;
    
    Serial.print("number of hit points = ");
    Serial.println( hitPoints );
    
    readFT5206TouchAddr( 0x03, tbuf, hitPoints*6);
    
    for (k=0,i = 0; (i < hitPoints*6)&&(k < num); k++, i += 6)
    {
      pLoc[k].x = (tbuf[i+0] & 0x0f) << 8 | tbuf[i+1];
      pLoc[k].y = (tbuf[i+2] & 0x0f) << 8 | tbuf[i+3];
    }
    
    retVal = k;
    
  } while (0);
  
  return retVal;
}

void writeFT5206TouchRegister( uint8_t reg, uint8_t val)
{
  Wire.beginTransmission(addr);
  Wire.write( reg );  // register 0
  Wire.write( val );  // value
  
  uint8_t retVal = Wire.endTransmission();  
}

void readOriginValues(void)
{
  writeFT5206TouchRegister(0, eTEST);
  delay(1);
  //uint8_t originLength = readFT5206TouchAddr(0x08, buf, 8 );
  uint8_t originXH = readFT5206TouchRegister(0x08);
  uint8_t originXL = readFT5206TouchRegister(0x09);
  uint8_t originYH = readFT5206TouchRegister(0x0a);
  uint8_t originYL = readFT5206TouchRegister(0x0b);
  
  uint8_t widthXH  = readFT5206TouchRegister(0x0c);
  uint8_t widthXL  = readFT5206TouchRegister(0x0d);
  uint8_t widthYH  = readFT5206TouchRegister(0x0e);
  uint8_t widthYL  = readFT5206TouchRegister(0x0f);
  
  //if (originLength)
  {
    Serial.print("Origin X,Y = ");
    Serial.print( uint16_t((originXH<<8) | originXL) );
    Serial.print(", ");
    Serial.println( uint16_t((originYH<<8) | originYL) );
    
    Serial.print("Width X,Y = ");
    Serial.print( uint16_t((widthXH<<8) | widthXL) );
    Serial.print(", ");
    Serial.println( uint16_t((widthYH<<8) | widthYL) );
  }
  
}




void setup()
{
  randomSeed(analogRead(0));
  
    Serial.begin(9600);
  Wire.begin();        // join i2c bus (address optional for master)
  
// Setup the LCD
  myGLCD.InitLCD();
 // -------------------------------------------------------------
  pinMode(8, OUTPUT);  //backlight 
  digitalWrite(8, HIGH);//on
// -------------------------------------------------------------
readOriginValues();
  pinMode     (FT5206_WAKE, INPUT);
  digitalWrite(FT5206_WAKE, HIGH );
  writeFT5206TouchRegister(0, eNORMAL); // device mode = Normal
  
  uint8_t periodMonitor = readFT5206TouchRegister(0x89);
  
  Serial.print("periodMonitor = ");
  Serial.println( periodMonitor, HEX );
  
  uint8_t  lenLibVersion = readFT5206TouchAddr(0x0a1, buf, 2 );
  if (lenLibVersion)
  {
    uint16_t libVersion = (buf[0] << 8) | buf[1];
  
    Serial.print("lib version = ");
    Serial.println( libVersion, HEX);
  }
  else
  {
    Serial.println("lib version length is zero");
  }
  
  uint8_t firmwareId = readFT5206TouchRegister( 0xa6 );
  
  Serial.print("firmware ID = ");
  Serial.println( firmwareId);
  
    pinMode     (FT5206_INT, INPUT);
  //digitalWrite(FT5206_INT, HIGH );

 
  myGLCD.setFont(SmallFont);
  
}

uint32_t dist(const TouchLocation & loc)
{
  uint32_t retVal = 0;
  
  uint32_t x = loc.x;
  uint32_t y = loc.y;
  
  retVal = x*x + y*y;
  
  return retVal;
}
uint32_t dist(const TouchLocation & loc1, const TouchLocation & loc2)
{
  uint32_t retVal = 0;
  
  uint32_t x = loc1.x - loc2.x;
  uint32_t y = loc1.y - loc2.y;
  
  retVal = sqrt(x*x + y*y);
  
  return retVal;
}

bool sameLoc( const TouchLocation & loc, const TouchLocation & loc2 )
{
  return dist(loc,loc2) < 50;
}


void loop()
{
  int buf[798];
  int x, x2;
  int y, y2;
  int r;

// Clear the screen and draw the frame
  myGLCD.clrScr();
  
 uint8_t flag = 1;
  static uint16_t w = 800;
  static uint16_t h = 480;
  
  float xScale = 1024.0F/w;
  float yScale = 1024.0F/h;
  
  uint8_t attention = digitalRead(FT5206_INT);
  static uint8_t oldAttention = 1;
  
  uint32_t thisTouchTime = millis();
  uint8_t i=0;   
  
  static 
  uint32_t lastTouchTime = thisTouchTime;
 
 
   
              myGLCD.setColor(0, 0, 0);
            myGLCD.fillRect(0, 0, 799, 479);
         myGLCD.setColor(255, 0, 0);
          myGLCD.print("* www.buydiplay.com  Capacitive touch screen test.Please touch the screen!", CENTER, 1);   
         myGLCD.setColor(0, 255, 255);
          myGLCD.print("*EXIT", LEFT, 460); 
 
while(flag)
 {  
  
    attention = digitalRead(FT5206_INT);
    
     /* Wait around for touch events */
    if (!attention && oldAttention ) 
    {   
      Serial.println("Touch: ");
      
      uint8_t count = readFT5206TouchLocation( touchLocations, 5 );


              
      //static uint8_t lastCount = count;
  
     if (count)
      {
        static TouchLocation lastTouch = touchLocations[0];
        
        if (((thisTouchTime - lastTouchTime) > 10000) && sameLoc( touchLocations[0],lastTouch ) )
        {
            myGLCD.setColor(0, 0, 0);
            myGLCD.fillRect(0, 0, 799, 479);
          lastTouchTime = thisTouchTime; 
   
        }
      
        Serial.print("Time delta = ");
        Serial.print(thisTouchTime - lastTouchTime);
        Serial.print(", dist = ");
        Serial.println( dist(touchLocations[0],lastTouch) );
        
        lastTouch = touchLocations[0];
        
        Serial.println("Locations: ");
        
        for (i = 0; i < count; i++)
        {

         myGLCD.setColor(255, 0, 0);
          myGLCD.print("* www.buydiplay.com  Capacitive touch screen test.Please touch the screen!", CENTER, 1);
          snprintf((char*)buf,sizeof(buf),"(%3d,%3d)",touchLocations[i].x,touchLocations[i].y);
          myGLCD.print((const char *)buf,CENTER,380+16*i);

        if(touchLocations[i].x>=3&&touchLocations[i].x<=40&&touchLocations[i].y>=450&&touchLocations[i].y<=480 )flag=0;

        if(i==0) 
        myGLCD.setColor(0, 0, 255),
        myGLCD.fillCircle(touchLocations[i].x,touchLocations[i].y, 5); 

        else if(i==1)  
         myGLCD.setColor(0, 255, 0),
        myGLCD.fillCircle(touchLocations[i].x,touchLocations[i].y, 5); 

        else if(i==2) 
         myGLCD.setColor(255, 0, 0), 
        myGLCD.fillCircle(touchLocations[i].x,touchLocations[i].y, 5 );   
               
        else if(i==3) 
         myGLCD.setColor(255, 255, 0),     
        myGLCD.fillCircle(touchLocations[i].x,touchLocations[i].y, 5 ); 

         else if(i==4) 
         myGLCD.setColor(255, 0, 255),       
         myGLCD.fillCircle(touchLocations[i].x,touchLocations[i].y, 5 ); 
    
        }
      }
        
    }
  
    else
    {
    }
    
    oldAttention = attention;
  }

  myGLCD.clrScr();
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 0, 799, 13);
  myGLCD.setColor(64, 64, 64);
  myGLCD.fillRect(0, 466, 799, 479);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.print("* EastRising Technology *", CENTER, 1);
  myGLCD.setBackColor(64, 64, 64);
  myGLCD.setColor(255,255,0);
  myGLCD.print(" http://www.buydisplay.com", CENTER, 467);

  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRect(0, 14, 799, 465);

// Draw crosshairs
  myGLCD.setColor(0, 0, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.drawLine(399, 15, 399, 464);
  myGLCD.drawLine(1, 239, 798, 239);
  for (int i=9; i<790; i+=10)
    myGLCD.drawLine(i, 237, i, 242);
  for (int i=19; i<470; i+=10)
    myGLCD.drawLine(397, i, 402, i);

// Draw sin-, cos- and tan-lines  
  myGLCD.setColor(0,255,255);
  myGLCD.print("Sin", 5, 15);
  for (int i=1; i<798; i++)
  {
    myGLCD.drawPixel(i,239+(sin(((i*1.13)*3.14)/180)*200));
  }
  
  myGLCD.setColor(255,0,0);
  myGLCD.print("Cos", 5, 27);
  for (int i=1; i<798; i++)
  {
    myGLCD.drawPixel(i,239+(cos(((i*1.13)*3.14)/180)*200));
  }

  myGLCD.setColor(255,255,0);
  myGLCD.print("Tan", 5, 39);
  for (int i=1; i<798; i++)
  {
    myGLCD.drawPixel(i,239+(tan(((i*0.9)*3.14)/180)));
  }

  delay(2000);

  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,798,464);
  myGLCD.setColor(0, 0, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.drawLine(399, 15, 399, 464);
  myGLCD.drawLine(1, 239, 798, 239);

// Draw a moving sinewave
  x=1;
  for (int i=1; i<(798*20); i++) 
  {
    x++;
    if (x==799)
      x=1;
    if (i>799)
    {
      if ((x==399)||(buf[x-1]==239))
        myGLCD.setColor(0,0,255);
      else
        myGLCD.setColor(0,0,0);
      myGLCD.drawPixel(x,buf[x-1]);
    }
    myGLCD.setColor(0,255,255);
    y=239+(sin(((i*1.65)*3.14)/180)*(200-(i / 100)));
    myGLCD.drawPixel(x,y);
    buf[x-1]=y;
  }

  delay(2000);
  
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,798,464);

// Draw some random filled rectangles
  for (int i=0; i<50; i++)
  {
    myGLCD.setColor(random(255), random(255), random(255));
    x=2+random(746);
    y=16+random(397);
    x2=x+50;
    y2=y+50;
    myGLCD.fillRect(x, y, x2, y2);
  }

  delay(2000);
  
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,798,464);

// Draw some random filled, rounded rectangles
  for (int i=0; i<50; i++)
  {
    myGLCD.setColor(random(255), random(255), random(255));
    x=2+random(746);
    y=16+random(397);
    x2=x+50;
    y2=y+50;
    myGLCD.fillRoundRect(x, y, x2, y2);
  }
  
  delay(2000);
  
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,798,464);

// Draw some random filled circles
  for (int i=0; i<50; i++)
  {
    myGLCD.setColor(random(255), random(255), random(255));
    x=27+random(746);
    y=41+random(397);
    myGLCD.fillCircle(x, y, 25);
  }
  
  delay(2000);
  
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,798,464);

// Draw some lines in a pattern
  myGLCD.setColor (255,0,0);
  for (int i=15; i<463; i+=5)
  {
    myGLCD.drawLine(1, i, (i*1.66)-10, 463);
  }
  myGLCD.setColor (255,0,0);
  for (int i=463; i>15; i-=5)
  {
    myGLCD.drawLine(798, i, (i*1.66)+30, 15);
  }
  myGLCD.setColor (0,255,255);
  for (int i=463; i>15; i-=5)
  {
    myGLCD.drawLine(1, i, 770-(i*1.66), 15);
  }
  myGLCD.setColor (0,255,255);
  for (int i=15; i<463; i+=5)
  {
    myGLCD.drawLine(798, i, 810-(i*1.66), 463);
  }
  
  delay(2000);
  
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,798,464);

// Draw some random circles
  for (int i=0; i<250; i++)
  {
    myGLCD.setColor(random(255), random(255), random(255));
    x=32+random(736);
    y=45+random(386);
    r=random(30);
    myGLCD.drawCircle(x, y, r);
  }

  delay(2000);
  
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,798,464);

// Draw some random rectangles
  for (int i=0; i<250; i++)
  {
    myGLCD.setColor(random(255), random(255), random(255));
    x=2+random(796);
    y=16+random(447);
    x2=2+random(796);
    y2=16+random(447);
    myGLCD.drawRect(x, y, x2, y2);
  }

  delay(2000);
  
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,798,464);

// Draw some random rounded rectangles
  for (int i=0; i<250; i++)
  {
    myGLCD.setColor(random(255), random(255), random(255));
    x=2+random(796);
    y=16+random(447);
    x2=2+random(796);
    y2=16+random(447);
    myGLCD.drawRoundRect(x, y, x2, y2);
  }

  delay(2000);
  
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,798,464);

  for (int i=0; i<250; i++)
  {
    myGLCD.setColor(random(255), random(255), random(255));
    x=2+random(796);
    y=16+random(447);
    x2=2+random(796);
    y2=16+random(447);
    myGLCD.drawLine(x, y, x2, y2);
  }

  delay(2000);
  
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,798,464);

  for (int i=0; i<10000; i++)
  {
    myGLCD.setColor(random(255), random(255), random(255));
    myGLCD.drawPixel(2+random(796), 16+random(447));
  }

  delay(2000);

  myGLCD.fillScr(0, 0, 255);
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRoundRect(320, 190, 479, 289);
  
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.print("www.buydisplay.com", CENTER, 195);
  myGLCD.print("That's it!", CENTER, 213);
  myGLCD.print("Restarting in a", CENTER, 239);
  myGLCD.print("few seconds...", CENTER, 252);
  
  myGLCD.setColor(0, 255, 0);
  myGLCD.setBackColor(0, 0, 255);
  myGLCD.print("Runtime: (msecs)", CENTER, 450);
  myGLCD.printNumI(millis(), CENTER, 465);
  
  delay (10000);
}

