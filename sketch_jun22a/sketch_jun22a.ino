#include "Arduino.h"
#include "I2Cdev.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"

#define I2C_ADDR          0x27        //Define I2C Address where the PCF8574A is
#define BACKLIGHT_PIN      3
#define En_pin             2
#define Rw_pin             1
#define Rs_pin             0
#define D4_pin             4
#define D5_pin             5
#define D6_pin             6
#define D7_pin             7
LiquidCrystal_I2C Serial_LCD_0(I2C_ADDR, En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);  //Serial LCD
 

void setup() {
  Serial_LCD_0.begin(16, 2);
  //Switch on the backlight
  Serial_LCD_0.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  Serial_LCD_0.setBacklight(HIGH);
  Serial_LCD_0.print(F("LCD OK...."));
}

void loop() {
   
   Serial_LCD_0.clear();
   Serial_LCD_0.print("MESG");
}
