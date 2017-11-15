#include "DspExpander.h"

boolean expok=false;



void dwrite(int p,int v) {
	
	
	if (expdr(p)==true) expander.digitalWrite(p-100,v);
		else digitalWrite(p,v);
}

int dread(int p) {
	
	
	if (expdr(p)==true) return expander.digitalRead(p-100);
		else return digitalRead(p);
}

void pmode(int p,int m) {
	if (useexpander and !expok) {expok=true;expander.begin(0x20);}
	
	if (expdr(p)) expander.pinMode(p-100,m);
		 else pinMode(p, m);
	
}

void dsppullUp(int p){
	if (expdr(p)) expander.pullUp(p-100);
		 else pinMode(p, INPUT_PULLUP);
	
}

void dsppullDown(int p){
	if (expdr(p)) expander.pullDown(p-100);
		 else ;
	
}

