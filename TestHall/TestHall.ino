#include <Narcoleptic.h>
#include <SFE_BMP180.h>
#include <RTClib.h>
#include <SPI.h>
#include <SD.h>
#include "DHT.h"

#define ALTITUDE 280.0 
#define usenarco 1

#define pAnmmeter 2
#define pAnmdeikt A0
#define pRainmeter A1
#define pDHTemp 8     // what digital pin we're connected to
#define CSPin 10


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
SFE_BMP180 bmp;
RTC_DS1307 RTC;      // RTC Module


volatile int cnt=0;
int cntpersec=0;
unsigned long mytime;
int anmdeik,avgWindSpeed;

double h,t,hic;
int rainmeter;//to ypsos ths broxhs
double dew,dewf;

double relP,Pres,bmpTemp;
DateTime now;

//---SD---
File myFile;


void aircnt()
{
  cnt++;  
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(pAnmdeikt,INPUT);
  pinMode(pAnmmeter,INPUT);
  attachInterrupt(digitalPinToInterrupt(pAnmmeter), aircnt, CHANGE);
  cntpersec=0;

  //--SD Card
  if (!SD.begin(CSPin)) 
    Serial.println(F("SDCard failed!"));
  else  
   Serial.println(F("SDCard OK."));
  
  dht.begin();


  if (bmp.begin()) 
    Serial.println(F("BMP180 success"));
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println(F("BMP180 fail\n\n"));    
  }  

    // Initialisiere RTC
  RTC.begin();

   // Prüfen ob RTC läuft  
  if (! RTC.isrunning()) {
    
    // Set the current date and time if the clock is not running yet
    RTC.adjust(DateTime(__DATE__, __TIME__));
    
    Serial.println(F("RTC was started and set to system time."));
  }
  else Serial.println(F("RTC is already running."));

  
  Serial.println("Setup ok");  
  mytime = millis();
}



//Celsius to Fahrenheit conversion
double Fahrenheit(double celsius)
{
  return 1.8 * celsius + 32;
}

//Celsius to Kelvin conversion
double Kelvin(double celsius)
{
  return celsius + 273.15;
}

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
    Serial.println(F("DHT sensor FAILED!"));
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

 if (( now.get() - prevtime)>secs ) {

   prevtime = now.get();
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
 //humidity,temperature,heat,rain,dewfast,dew,pressure,rel pressure,temperature,wind speed, wind avg,wind dir
  wfile(h);wfile(t);wfile(hic);wfile(rainmeter);wfile(dewf);wfile(dew);wfile(Pres);wfile(relP);wfile(bmpTemp);wfile(cntpersec);wfile(avgWindSpeed);wfile(getWindDirection(anmdeik));
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
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(F("Heat index: "));
  Serial.print(hic);
  Serial.println(" *C ");  
  Serial.print(F("Rain Height: "));  Serial.print(rainmeter);
  Serial.print(F("\tDewFast: "));  Serial.print(dewf);
  Serial.print(F("\tDew: "));  Serial.print(dew);
  Serial.println(" ");
  Serial.print(F("Absolute pressure: "));
  Serial.print(Pres,2);
  Serial.print(" mb, ");
  Serial.print(F("Relative (sea-level) pressure: "));
  Serial.print(relP,2);
  Serial.print(F(" mb, "));
  Serial.print(F("Temperature: "));
  Serial.print(bmpTemp);
  Serial.println(" ");
}

void prnDateTime() {

 // Wochentag, Tag.Monat.Jahr
  Serial.print(get_day_of_week(now.dayOfWeek()));
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

    Serial.print(F("Wind speed:"));Serial.print(cntpersec);  Serial.print(F("  "));
    Serial.print(F("Average wind speed:"));Serial.println(avgWindSpeed);        
    Serial.print(F("Direction:"));Serial.print(anmdeik); Serial.print(F("("));
    prnWindDirection(getWindDirection(anmdeik));Serial.print(F(")"));
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

void loop() {
  // put your main code here, to run repeatedly:
  

 if (millis()-mytime>=1000) {     
    cntpersec=cnt;cnt=0;mytime = millis();
    avgWindSpeed=(avgWindSpeed+cntpersec) / 2;
  }  

if (everySecs(5)) {
  getHumTemp();
  getRainMeter();
  dewf=dewPointFast(t,h);
  dew=dewPoint(t,h);
  getPressure();
  anmdeik=getAnemodeikt();
  getDTime();

  prnDateTime();
  prnSensors();
  prnAnemo();

  Serial.println("");
  Serial.println("======================================= ");

  saveToFile();
  
  if (usenarco==1) {
     Narcoleptic.delay(10000);
     everySecs(0);//reset timer
  }
 }

 
  
  
}
