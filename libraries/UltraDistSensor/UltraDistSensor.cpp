/*
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
 */

#include <Arduino.h>
#include "UltraDistSensor.h"


void UltraDistSensor::attach(uint8_t SignalPin)
{
	UltraDistSensor::attach(SignalPin,SignalPin);
}

void UltraDistSensor::attach(uint8_t TrigerPin,uint8_t EchoPin,uint16_t timeout)
{
	trig=TrigerPin;
	echo=EchoPin;
	timeOut=timeout;
	common= trig==echo ? true:false;
	pinMode(trig, OUTPUT);
	pinMode(echo, INPUT);
}

float UltraDistSensor::distanceInCm()
{
	return (getReading()/2)/29.1;
}

float UltraDistSensor::distanceInInch(){
	
	return (getReading()/2)/73.9;
}
int UltraDistSensor::getReading(){
	delay(10);
	if(common==true) pinMode(trig,OUTPUT);
	digitalWrite(trig, LOW);
	delayMicroseconds(2);
	digitalWrite(trig, HIGH);
	delayMicroseconds(10);
	digitalWrite(trig, LOW);
	if(common==true) pinMode(echo, INPUT);
	return pulseIn(echo, HIGH,timeOut);
}
void UltraDistSensor::changeTimeout(uint16_t time){
	timeOut=time;
}
