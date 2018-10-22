
volatile int x;

void speedencint()//attach change photointerrupt here (Wrapper function for my class)
{
  x++;  
}

void setup() {
   Serial.begin(115200);                                                 
  Serial.println(F("Serial comm ok!"));   
  // put your setup code here, to run once:
  pinMode (3, INPUT) ;
  attachInterrupt(digitalPinToInterrupt(3), speedencint, CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:
 Serial.println(x);
 delay(1000);
}
