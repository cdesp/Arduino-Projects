#include <DspGyro2.h>

DESP_Gyro gyro(0);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  gyro.init();
  Serial.write("Setup OK");  
  
}

void loop() {
  // put your main code here, to run repeatedly:
  gyro.getSafeGyroPos();
  Serial.println(gyro.curbearing);
}
