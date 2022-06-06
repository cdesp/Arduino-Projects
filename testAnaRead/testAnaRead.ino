void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(14, INPUT); 

}

void loop() {
  // put your main code here, to run repeatedly:
  int t = analogRead(14);
  Serial.print("ANIN:");
  Serial.println(t);
  delay(100);
}
