#ifndef DspExpander_H
	#define DspExpander_H
	
	#include <Wire.h>    // Required for I2C communication
	#include <PCF8575.h> // Required for PCF8575
	#include <Arduino.h>

/** PCF8575 instance */
extern PCF8575 expander;

#define epin(n) 100+n
#define expdr(p) (p>=100)



extern boolean useexpander;


void dwrite(int p,int v);

int dread(int p);

void pmode(int p,int m) ;

void dsppullUp(int p);

void dsppullDown(int p);

#endif
