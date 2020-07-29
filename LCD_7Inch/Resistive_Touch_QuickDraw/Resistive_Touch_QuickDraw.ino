/***************************************************
//Web: http://www.buydisplay.com
EastRising Technology Co.,LTD
Examples for ER-TFTM070-4V2.1(800x480 Pixels) with Resistive Touch Panel
Software SPI,8080 16-bit Interface 5V Power Supply

This program is a quick demo of how to use the library.
This program requires the UTFT library.
It is assumed that the display module is connected to an
appropriate shield or that you know how to change the pin 
numbers in the setup.

Tested and worked with:
Arduino Mega 2560,Arduino Due
Works with Arduino 1.6.0 IDE 
****************************************************/

#include <UTFT.h>
#include <UTouch.h>

// Set the pins to the correct ones for your development shield
// ------------------------------------------------------------
// Standard Arduino Mega/Due shield            : <display model>,38,39,40,41
UTFT myGLCD(SSD1963_800480,38,39,40,41);  //(byte model, int RS, int WR, int CS, int RST, int SER)
UTouch  myTouch( 43, 42, 44, 45, 46);  //byte tclk, byte tcs, byte din, byte dout, byte irq

extern uint8_t SmallFont[];

void setup()
{

  myGLCD.InitLCD();
  // -------------------------------------------------------------
  pinMode(8, OUTPUT);  //backlight 
  digitalWrite(8, HIGH);//on
// -------------------------------------------------------------
  myGLCD.clrScr();

  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);
    myGLCD.setFont(SmallFont);
}

void loop()
{
  long x, y;
         myGLCD.setColor(255, 0, 0);
          myGLCD.print("* www.buydiplay.com  Resistive touchscreen test.Please touch the screen!", CENTER, 1);   
  while (myTouch.dataAvailable() == true)
  {
    myTouch.read();
    x = myTouch.getX();
    y = myTouch.getY();
   // if ((x!=-1) and (y!=-1))
    {
      myGLCD.drawPixel (x, y);
    }
  }
}

