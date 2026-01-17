/* This code works with JSN SR04 T ultrasound module
2 * It measures  the distance and shows it on the Serial monitor
3 * Refer to www.SurtrTech. com  or SurtrTech YT channel for more informations 
4 */

#define TRIG 17//12 //Module  pins
#define ECHO 18//13 

void setup() { 
  
  Serial.begin(115200); //  Serial monitoring 
  pinMode(TRIG, OUTPUT); // Initializing Trigger Output and  Echo Input 
  pinMode(ECHO, INPUT_PULLUP);
  } 
  
  void loop() { 
    
    digitalWrite(TRIG, LOW); // Set the trigger pin to low for 2uS 
    delayMicroseconds(2); 
    
    digitalWrite(TRIG, HIGH); // Send a 10uS  high to trigger ranging 
    delayMicroseconds(20); 
    
    digitalWrite(TRIG,  LOW); // Send pin low again 
    float distance = pulseIn(ECHO, HIGH,26000);  // Read in times pulse 
    
    distance= distance/58; //Convert the pulse  duration to distance
                           //You can add other math functions  to calibrate it well
   // distance= distance*0.3937; //Convert from cm to inches
    
    Serial.print("Distance ");
    Serial.print(distance,2);
    Serial.println("  CMs");
   
    delay(50);
    
}
