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






#ifndef UltraDistSensor_h
#define UltraDistSensor_h

class UltraDistSensor {
	private:
		uint8_t trig,echo;
		bool common;
		uint16_t timeOut;
		int getReading();
	public:
		UltraDistSensor(){};
		void attach(uint8_t SignalPin);
	void attach(uint8_t TrigerPin,uint8_t EchoPin,uint16_t timeout=20000ul);
		float distanceInCm();
		float distanceInInch();
		void changeTimeout(uint16_t time);
		
};

#endif
