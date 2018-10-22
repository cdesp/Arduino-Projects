#include <TinyGPS++.h>
//#include <SoftwareSerial.h>

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address, if it's not working try 0x27.

/*
   This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
static const int RXPin = 4, TXPin = 3;
//static const uint32_t GPSBaud = 4800;
static const uint32_t GPSBaud = 9600; //Replace with your baud rate

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
//SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  Serial.begin(115200);
  //ss.begin(GPSBaud);
  Serial3.begin(GPSBaud);
  

  Serial.println(F("DeviceExample.ino"));
  Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();

  lcd.begin(16,2);   // iInit the LCD for 16 chars 2 lines
  lcd.backlight();   // Turn on the backligt (try lcd.noBaklight() to turn it off)
  lcd.setCursor(0,0); //First line
  lcd.print("GPS TEST");
  lcd.setCursor(0,1); //Second line
  lcd.print("Starting...");
  delay(2000);
  lcd.clear();
}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (Serial3.available() > 0)
    if (gps.encode(Serial3.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    lcd.setCursor(0,1); //Second line
    lcd.print(F("No GPS detected: check wiring."));
    while(true);
  }
}

void displayInfo()
{
  Serial.print(F("Location: ")); 
  lcd.setCursor(0,0); //First line
  lcd.print(F("L: "));

  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
    lcd.print(gps.location.lat(), 6);
    lcd.print(F(","));
    lcd.print(gps.location.lng(), 6);
  }
  else
  {
    lcd.print(F("INVALID"));
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  lcd.setCursor(0,1); //Second line
//  lcd.print(F("Dt/Tm: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
//    lcd.print(gps.date.month());
//    lcd.print(F("/"));
//    lcd.print(gps.date.day());
//    lcd.print(F("/"));
//    lcd.print(gps.date.year());
    
  }
  else
  {
//    lcd.print(F("INVALID"));
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
 // lcd.print(F("Tm: "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) {Serial.print(F("0"));lcd.print(F("0"));}
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    lcd.print(gps.time.hour());
    lcd.print(F(":"));    
    if (gps.time.minute() < 10) {Serial.print(F("0"));lcd.print(F("0"));}
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    lcd.print(gps.time.minute());
    lcd.print(F(":"));    
    if (gps.time.second() < 10) {Serial.print(F("0"));lcd.print(F("0"));}
    Serial.print(gps.time.second());
    Serial.print(F("."));
    lcd.print(gps.time.second());
  //  lcd.print(F("."));
    if (gps.time.centisecond() < 10) {Serial.print(F("0"));}//lcd.print(F("0"));}
    Serial.print(gps.time.centisecond());
    //lcd.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
    lcd.print(F("INVALID"));
  }
  Serial.print(F(" "));
  Serial.print("Sats:");
  Serial.print(gps.satellites.value());
  lcd.print(F(" - "));lcd.print(gps.satellites.value());
  Serial.println();
}
