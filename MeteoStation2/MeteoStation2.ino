//#include <Narcoleptic.h>
//#include <SFE_BMP180.h>
#include <Adafruit_BMP280.h>
#include <SPI.h>
#include <SD.h>
#include "DHT.h"
#include <RTClib.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <EEPROM.h>


#define ON HIGH
#define OFF LOW

#define ALTITUDE 280.0 

#define pAnmmeter 2
#define pAnmdeikt A0
#define pRainmeter A1
#define pDHTemp 6//9  //8   // what digital pin we're connected to
#define CSPin 10
#define btoothrx 5
#define btoothtx 4
#define powerPin 8  //controls power to the sensors


// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(pDHTemp, DHTTYPE);
//SFE_BMP180 bmp;
Adafruit_BMP280 bmp; // I2C Interface
RTC_DS1307 RTC;      // RTC Module



volatile int cnt=0;
int cntpersec=0;
int times;
unsigned long mytime;
int anmdeik,avgWindSpeed,maxWindSpeed,sumWindSpeed;

double h,t,hic;
int rainmeter;//to ypsos ths broxhs
double dew,dewf;

double relP,Pres,bmpTemp;
DateTime now;
int sleeptime=30; //the minutes to sleep
int fileevery=1; //the minutes  to write to file after wake up means sampling sensors for fileevery minutes then goto sleep
int airevery=10; //the seconds to  calc the air speed 10 means sample for 10 secs then  do the calcs

//---SD---
File myFile;


void aircnt()
{
  cnt++;  
}


void mysetup(){
  
  Serial.begin(115200);
  Serial.println("S"); 
  pinMode(powerPin,OUTPUT);
  powerSensors(ON);    
  delay(4000);  
 
  pinMode(pAnmdeikt,INPUT);
  pinMode(pAnmmeter,INPUT);  
  attachInterrupt(digitalPinToInterrupt(pAnmmeter), aircnt, CHANGE);
  cntpersec=0;


  
  //--SD Card
  if (!SD.begin(CSPin)) 
    Serial.println(F("SD err!"));
  //else  
  // Serial.println(F("SDCard OK."));
  
  dht.begin();
//Serial.println("B"); 


  //if (bmp.begin()) 
 //   Serial.println(F("BMP180 success"));
 // else
 // {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

 //   Serial.println(F("BMP180 fail\n\n"));    
 // }  

  
  if (!bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID)) {
    Serial.println(F("BMP280 err!"));
  }
  else {
    //Serial.println(F("BMP280 ok!"));
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  }

//Serial.println("R");
    // Initialisiere RTC

  RTC.begin();

   // Prüfen ob RTC läuft  
  if (! RTC.isrunning()) {
    
    // Set the current date and time if the clock is not running yet
    RTC.adjust(DateTime(__DATE__, __TIME__));
    
    Serial.println(F("RTC no bat?"));
  }
  //else Serial.println(F("RTC ok."));

  
  Serial.println("OK");    
  mytime = millis();
  everySecs(0);  
  times=0;
}


void setup(){
  int t1;  

  wdt_disable();
  t1=EEPROM.read(0);
  if (t1> sleeptime*8)  {  //8 times for a minute
    EEPROM.put(0, 0);
    mysetup();
    wdt_enable(WDTO_8S);
  }
  else {
    Serial.begin(115200);
    EEPROM.put(0, t1+1); //increase the value
    Serial.println(t1); 
    wdt_enable(WDTO_8S);   
    gotosleep();  //go back to sleep
    
  }    
  
}


//Celsius to Fahrenheit conversion
//double Fahrenheit(double celsius)
//{
//  return 1.8 * celsius + 32;
//}

//Celsius to Kelvin conversion
//double Kelvin(double celsius)
//{
 // return celsius + 273.15;
//}

/*
// dewPoint function NOAA
// reference: http://wahiduddin.net/calc/density_algorithms.htm 
double dewPoint(double celsius, double humidity)
{
  double A0= 373.15/(273.15 + celsius);
  double SUM = -7.90298 * (A0-1);
  SUM += 5.02808 * log10(A0);
  SUM += -1.3816e-7 * (pow(10, (11.344*(1-1/A0)))-1) ;
  SUM += 8.1328e-3 * (pow(10,(-3.49149*(A0-1)))-1) ;
  SUM += log10(1013.246);
  double VP = pow(10, SUM-3) * humidity;
  double T = log(VP/0.61078);   // temp var
  return (241.88 * T) / (17.558-T);
}
*/
// delta max = 0.6544 wrt dewPoint()
// 5x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
  double a = 17.271;
  double b = 237.7;
  double temp = (a * celsius) / (b + celsius) + log(humidity/100);
  double Td = (b * temp) / (a - temp);
  return Td;
}


boolean getHumTemp(){

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) ) {
   // Serial.println(F("DHT err"));
    return false;
  }
  // Compute heat index in Celsius (isFahreheit = false)
  hic = dht.computeHeatIndex(t, h, false);

  return true;
}


void getRainMeter(){
  // read the input on analog pin :
  rainmeter = analogRead(pRainmeter);
}

//for bmp280
void getPressure280(){
  double T,P,p0,a;

  bmp.takeForcedMeasurement();
  bmpTemp=bmp.readTemperature();
  P=bmp.readPressure();
  Pres=P/100;
  a=bmp.readAltitude(1019.66);
  relP = bmp.seaLevelForAltitude(ALTITUDE,P)/100;

}

/* for bmp180
void getPressure(){
  char status;
  double T,P,p0,a;


  status = bmp.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.

    status = bmp.getTemperature(T);
    if (status != 0)
    {
      // Print out the measurement:
    //  Serial.print("temperature: ");
    //  Serial.print(T,2);
    //  Serial.print(" deg C, ");
    //  Serial.print((9.0/5.0)*T+32.0,2);
    //  Serial.println(" deg F");

      bmpTemp=T;
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = bmp.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = bmp.getPressure(P,T);
        if (status != 0)
        {
          // Print out the measurement:
         // Serial.print("absolute pressure: ");
         // Serial.print(P,2);
         // Serial.print(" mb, ");
         // Serial.print(P*0.0295333727,2);
         // Serial.println(" inHg");

          Pres=P;
          // The pressure sensor returns abolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sealevel function and your current altitude.
          // This number is commonly used in weather reports.
          // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
          // Result: p0 = sea-level compensated pressure in mb

          p0 = bmp.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          //Serial.print("relative (sea-level) pressure: ");
          //Serial.print(p0,2);
          //Serial.print(" mb, ");
          //Serial.print(p0*0.0295333727,2);
          //Serial.println(" inHg");

          relP=p0;
          
          
          // On the other hand, if you want to determine your altitude from the pressure reading,
          // use the altitude function along with a baseline pressure (sea-level or other).
          // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
          // Result: a = altitude in m.

       //   a = bmp.altitude(P,p0);
        //  Serial.print("computed altitude: ");
         // Serial.print(a,0);
         // Serial.print(" meters, ");
         // Serial.print(a*3.28084,0);
         // Serial.println(" feet");}
        }
        else Serial.println(F("error retrieving pressure\n"));
      }
      else Serial.println(F("error starting pressure\n"));
    }
    else Serial.println(F("error retrieving temperature\n"));
  }
  else Serial.println(F("error starting temperature\n"));

 
}

*/

void getDTime(){

  now=RTC.now(); // aktuelle Zeit abrufen
   
}


// Wochentag ermitteln
String get_day_of_week(uint8_t dow){ 
  
  String dows="  ";
  switch(dow){
   case 0: dows="Su"; break;
   case 1: dows="Mo"; break;
   case 2: dows="Tu"; break;
   case 3: dows="We"; break;
   case 4: dows="Th"; break;
   case 5: dows="Fr"; break;
   case 6: dows="Sa"; break;
  }
  
  return dows;
}



unsigned long prevtime=0;

bool everySecs(int secs){

 getDTime();

 if (( now.unixtime() - prevtime)>secs ) {

   prevtime = now.unixtime();
   return true;
 }


 return false; 
}


int getAnemodeikt(){
  return analogRead(pAnmdeikt);  
}


int getWindDirection(int a){


  return map(a,0,1023,0,7);
  //a (0..1023)
/*  switch(a){  
    case 0..127:return 0;
    case 128..255:return 1;
    case 256..383:return 2;
    case 384..511:return 3;
    case 512..639:return 4;
    case 640..767:return 5;
    case 768..895:return 6;
    case 896..1024:return 7;
  }
  */
}

//----------------------------
//SD Card

void wfile(double v){
  myFile.print(",");myFile.print(v);
}

void wDateTime(){
  if(now.day()<10)myFile.print(0);
  myFile.print(now.day(),DEC);
  myFile.print(".");
  if(now.month()<10)myFile.print(0);
  myFile.print(now.month(),DEC);
  myFile.print(".");
  myFile.print(now.year(),DEC);
  myFile.print(",");
  // Stunde:Minute:Sekunde
  if(now.hour()<10)myFile.print(0);
  myFile.print(now.hour(),DEC);
  myFile.print(":");
  if(now.minute()<10)myFile.print(0);
  myFile.print(now.minute(),DEC);
  myFile.print(":");
  if(now.second()<10)myFile.print(0);
  myFile.print(now.second(),DEC);
}

void writetofile(){
  //print time p.x. 29.07.2018,14:28:49
  wDateTime(); 
 //humidity,temperature,heat,rain,dewfast,dew,pressure,rel pressure,temperature,max wind speed per sec, wind avg per sec,wind dir
  wfile(h);wfile(t);wfile(hic);wfile(rainmeter);wfile(dewf);wfile(dewf);wfile(Pres);wfile(relP);wfile(bmpTemp);wfile(maxWindSpeed);wfile(avgWindSpeed);wfile(getWindDirection(anmdeik));
  myFile.println("");
}



void saveToFile(){
  char fn[20],tm[3],td[3];

  if(now.day()<10) sprintf(tm, "0%d",now.day()); else sprintf(td, "%d",now.day());
  if(now.month()<10) sprintf(tm, "0%d",now.month()); else sprintf(tm, "%d",now.month());
  
  sprintf(fn, "%d%s%s.txt", now.year(),tm,td);
  //Serial.println(fn);  
  myFile = SD.open(fn, FILE_WRITE); 
  writetofile();
  myFile.close(); 
}

//===================================
void prnSensors(){
  Serial.print(F("Hum "));
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print(F("T "));
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(F("Heat "));
  Serial.print(hic);
  Serial.println(" *C ");  
  Serial.print(F("Rn : "));  Serial.print(rainmeter);
  Serial.print(F("\tDew: "));  Serial.print(dewf);
 // Serial.print(F("\tDew: "));  Serial.print(dew);
  Serial.println(" ");
  Serial.print(F("pr: "));
  Serial.print(Pres,2);
  //Serial.print(" mb, ");
  Serial.print(F("pr R: "));
  Serial.print(relP,2);
 // Serial.print(F(" mb, "));
  Serial.print(F("T2: "));
  Serial.print(bmpTemp);
  Serial.println(" ");
}

void prnDateTime() {

 // Wochentag, Tag.Monat.Jahr
  Serial.print(get_day_of_week(now.dayOfTheWeek()));
  Serial.print(", ");
  if(now.day()<10)Serial.print(0);
  Serial.print(now.day(),DEC);
  Serial.print(".");
  if(now.month()<10)Serial.print(0);
  Serial.print(now.month(),DEC);
  Serial.print(".");
  Serial.print(now.year(),DEC);
  Serial.print("    ");
  // Stunde:Minute:Sekunde
  if(now.hour()<10)Serial.print(0);
  Serial.print(now.hour(),DEC);
  Serial.print(":");
  if(now.minute()<10)Serial.print(0);
  Serial.print(now.minute(),DEC);
  Serial.print(":");
  if(now.second()<10)Serial.print(0);
  Serial.println(now.second(),DEC);
  
}

void prnAnemo(){

    Serial.print(("Wind spd:"));Serial.print(cntpersec);  Serial.print(("  "));
  //  Serial.print(("Avg :"));Serial.print(avgWindSpeed); Serial.print(("  "));        
   // Serial.print(("Max :"));Serial.println(maxWindSpeed);        
   // Serial.print(("Dir:"));Serial.print(anmdeik); Serial.print(("("));
    prnWindDirection(getWindDirection(anmdeik));Serial.print((")"));
}

void prnWindDirection(int w){
  switch(w){
    case 0:Serial.print(F("N"));break;
    case 1:Serial.print(F("NE"));break;
    case 2:Serial.print(F("E"));break;
    case 3:Serial.print(F("SE"));break;
    case 4:Serial.print(F("S"));break;
    case 5:Serial.print(F("SW"));break;
    case 6:Serial.print(F("W"));break;
    case 7:Serial.print(F("NW"));break;
  }
  
}

void getReadings(){
  getHumTemp();
  getRainMeter();
  dewf=dewPointFast(t,h);
  //dew=dewPoint(t,h);
  getPressure280();
  anmdeik=getAnemodeikt();
  getDTime();
}

void  prnDataToSerial(){
  prnDateTime();
  prnSensors();
  prnAnemo();

  Serial.println("");
  Serial.println("");
  
}


void gotosleep(){
  wdt_reset();
 // Serial.println("slp");
 // Serial.flush();  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); 
  sleep_enable();
 // Now enter sleep mode. 
  sleep_mode();  
 // Serial.println("wake");
 // Serial.flush();    
}

void powerSensors(uint8_t pwrstatus){
  digitalWrite(powerPin,pwrstatus);   
  if (pwrstatus==ON) {
    delay(2000);//delay to power on the devices
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  String incoming,dts,tms,hs,ms,ys,mns,ds;
  int t1;

  wdt_reset();
  
  if (Serial.available() > 0) {
    // read the incoming byte:
    
    incoming = Serial.readString();
      Serial.println(incoming);
      //D26/07/2022 19:07
    if (incoming.charAt(0)=='D'){
      dts=incoming.substring(1,11);
      ds=dts.substring(0,2);
      mns=dts.substring(3,5);
      ys=dts.substring(6,10);
      tms=incoming.substring(12);
      hs=tms.substring(0,2);
      ms=tms.substring(3,5);
      //Serial.println(dts);
      //Serial.println(tms);    

    //  Serial.println(ys.toInt());
      dts=ds+"/"+mns+"/"+ys+" ";
      tms=hs+":"+ms;
      RTC.adjust(DateTime(ys.toInt(),mns.toInt(),ds.toInt(),hs.toInt(),ms.toInt(),0));
      
      Serial.print(dts);
      Serial.println(tms);
      Serial.println("set");
     }         
     if (incoming.charAt(0)=='P'){ 
      getReadings();
      prnDataToSerial();
     }
  }


 //avg air  speed calculations
 if (millis()-mytime>=airevery*1000) {     //secs time to calc air  speed
    cntpersec=cnt/airevery ;cnt=0;mytime = millis();
    sumWindSpeed=sumWindSpeed+cntpersec ;
    avgWindSpeed=sumWindSpeed/++times;
    if (maxWindSpeed<cntpersec)
      maxWindSpeed=cntpersec;
  }  

if (everySecs(fileevery*60)) {  //mins save data?

  wdt_reset();
  getReadings();
  wdt_reset();
  prnDataToSerial();
  wdt_reset();
  saveToFile();
  wdt_reset();
  
  maxWindSpeed=0; times=0; 
  powerSensors(OFF);
  gotosleep();
  //Serial.println("wake");
 }

 
  
  wdt_reset();
}
