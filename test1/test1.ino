

float pollapl(float t,float epi=1.5){

  return t*epi;
}

void setup() {
  Serial.begin(115200);
for(float x = 2; x < 100; x=pollapl(x,2) ){
 Serial.println(x);
}
}

void loop() {
 
}
