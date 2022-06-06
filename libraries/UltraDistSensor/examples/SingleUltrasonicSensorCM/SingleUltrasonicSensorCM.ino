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
UltraDistSensor mysensor;
float reading;
void setup() {
    Serial.begin(9600);
    mysensor.attach(12,13);//Trigger pin , Echo pin
}

void loop() {
    reading=mysensor.distanceInCm();
    Serial.print("Sensor Reading :");Serial.print(reading);Serial.println(" CM");
    delay(1000);
}
