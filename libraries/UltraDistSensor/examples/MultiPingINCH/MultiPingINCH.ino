/*
 * This program reads the distance from multiple ping sensors 
 * and produce output in the form of Inches. 
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
 * |  Ping   | Arduino | 
 * |_________|_________|
 * |   Vcc   |   5V    |     
 * |   sig   |    2    |
 * |   Gnd   |   GND   |
 * |_________|_________|
 *  ___________________
 * |  Ping   | Arduino | 
 * |_________|_________|
 * |   Vcc   |   5V    |     
 * |   sig   |    4    |
 * |   Gnd   |   GND   |
 * |_________|_________|
 *  ___________________
 * |  Ping   | Arduino | 
 * |_________|_________|
 * |   Vcc   |   5V    |     
 * |   sig   |    6    |
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
 * which will be having plus or minus tolernace upto 1cm or inch. 
 */
#include<UltraDistSensor.h>
UltraDistSensor mysensor1;
UltraDistSensor mysensor2;
UltraDistSensor mysensor3;
float reading;
void setup() {
    Serial.begin(9600);
    mysensor1.attach(2);//signal pin 
    mysensor2.attach(4);//signal pin 
    mysensor3.attach(6);//signal pin 
}

void loop() {
    reading=mysensor1.distanceInInch();
    Serial.print("Sensor 1 Reading :");
    Serial.print(reading);
    Serial.println(" Inches");
    reading=mysensor2.distanceInCm();
    Serial.print("Sensor 2 Reading :");
    Serial.print(reading);
    Serial.println(" Inches");
    reading=mysensor3.distanceInCm();
    Serial.print("Sensor 3 Reading :");
    Serial.print(reading);
    Serial.println(" Inches");
    delay(1000);
}
