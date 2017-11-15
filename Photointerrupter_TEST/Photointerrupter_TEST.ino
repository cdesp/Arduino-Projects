
int intpin=2;
volatile int icnt=0;
boolean forward=true;

void myISR(){
  if (forward)
    icnt++;
  else
    icnt--;  
}

void setup() {
  // put your setup code here, to run once:
  pinMode (intpin, INPUT_PULLUP) ;// define the photo interrupter sensor output interface
  attachInterrupt(digitalPinToInterrupt(intpin), myISR, RISING  );
  Serial.begin(9600);
}

int val;
void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("COUNT=");
  Serial.print(icnt);
  Serial.println("");
  delay(1000);
}


