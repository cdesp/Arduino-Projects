/*
 * This program reads the distance from single hc-sr04 sensor
 * and produce output in the form of Centimeters.
 * 
 * UltraDistSensor is library designed to Interface 
 * HC-SR04 Ultrasonics sensor to Arduino microcontrollers.
 * 
 * Designed and developed by
 * 
 * Shubham Trivedi on Oct 2017
 * 
 * No license required to access this library it is release into open source 
 * license
 * for any query you can write me at shubhamtrivedi95@gmail.com
 * follow me on facebook https://www.facebook.com/shubhamtrivedi95
 * on github https://www.github.com/shubhamtrivedi95
 * for latest updates subscribe at http://electro-passionindia.blogspot.in
 *   
 *   Pin configuration
 *  ___________________
 * | HC-SC04 | Arduino | 
 * |_________|_________|
 * |   Vcc   |   5V    |     
 * |   Trig  |   12    |
 * |   Echo  |   13    | 
 * |   Gnd   |   GND   |
 * |_________|_________|
 * 
 * attach()         This function configure sensor with arduino
 * 
 * distanceInCm()   This function produce distance in centimeter
 * 
 * distanceInInch() This function produce distance in Inches 
 * 
 * ChangeTimeout()  This functionis used to change echo timeout of receiveing pulse
 *                  by default the timeout is 20000 microseconds
 * Please note that you will get Approximated distance from HC-SR04 module, 
 * which will be having plus or minus 2% tolernace. 
 */
#include<UltraDistSensor.h>

const int buzzer = 7; //buzzer to arduino pin 9

UltraDistSensor mysensor;
float reading;
void setup() {
    Serial.begin(115200);
    mysensor.attach(2,3);//Trigger pin , Echo pin
    pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
    
}

void loop() {
    reading=mysensor.distanceInCm();
   // Serial.print("Sensor Reading :");Serial.print(reading);Serial.println(" CM");
   if (reading<4) {
     tone(buzzer, 7000); // Send 1KHz sound signal...   
   }  
   else    if (reading<10) {
     tone(buzzer, 5000); // Send 1KHz sound signal...   
   } else   if (reading<16) {
     tone(buzzer, 2500); // Send 1KHz sound signal...   
   }  else    if (reading<22) {
     tone(buzzer, 1200); // Send 1KHz sound signal...        
   }  else    if (reading<28) {
     tone(buzzer, 500); // Send 1KHz sound signal...   
   } else noTone(buzzer);     // Stop sound... 




    
    //delay(100);
}
