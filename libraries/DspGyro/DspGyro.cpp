#include "DspGyro.h"


 


DESP_Gyro::DESP_Gyro(int addr) {
  
  
  mpu= new MPU6050(addr);
  
  redogyro=0;
  turning=false;
  dmpReady=false;
  targbearing=0;
  yprpre[0]=100;
}



void DESP_Gyro::checkGyro(boolean debug){
  
  int safecnt=0;
    // if programming failed, don't try to do anything
  if (!dmpReady) return;
 
 //for (int i=0;i++<2;){ 
    //if (debug)
     // Serial.println(F("Check gyro"));
   mpuIntStatus=0;
   mpu->resetFIFO();
   
   
   
   while(!(mpuIntStatus & 0x02)){
    if (safecnt++>5) { ypr[0]=0;delay(15);break; }//for safety
    
    mpuIntStatus = mpu->getIntStatus();

    // get current FIFO count
    
    fifoCount = mpu->getFIFOCount();
   

    // check for overflow (this should never happen unless our code is too inefficient)
    
    if ((mpuIntStatus & 0x10) || fifoCount >= 1024) {
        // reset so we can continue cleanly
        mpu->resetFIFO();
       // if (debug)
        //  Serial.println(F("FIFO overflow!"));       
    } 
   }//while end
  // Serial.println("g3");
//}//for end
 
    if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        int safecnt=0;
        while (fifoCount < packetSize) {fifoCount = mpu->getFIFOCount();
        if (++safecnt>20) {
        	 //Serial.println("safecnt over 20");
        	 break;
        } else delay(5);}
			//	Serial.println("g3T1");
			//	Serial.println(fifoCount,DEC);
        // read a packet from FIFO
        
        mpu->getFIFOBytes(fifoBuffer, packetSize);
       // Serial.println("g3T2");
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

            
            mpu->dmpGetQuaternion(&q, fifoBuffer);

            mpu->dmpGetGravity(&gravity, &q);

            mpu->dmpGetYawPitchRoll(ypr, &q, &gravity);

            
         /*  if (debug && ypr[0]!=180 && ypr[0]!=0){
            Serial.print("ypr\t");
            Serial.print(ypr[0] * 180/M_PI);
            Serial.print("\t");
            Serial.print(ypr[1] * 180/M_PI);
            Serial.print("\t");
            Serial.println(ypr[2] * 180/M_PI);
              }*/
            ypr[0]*= 180/M_PI;
            ypr[1]*= 180/M_PI;
            ypr[2]*= 180/M_PI;            

    }
  //  Serial.println("g4");
   mpu->resetFIFO(); 
  
  
}

void DESP_Gyro::calibrateGyro(boolean debug){
  delay(200);
  getSafeGyroPos();  
  while (abs(yprpre[0]-ypr[0])>0.1 and ypr[0]!=0){
    yprpre[0]=ypr[0];
    yprpre[1]=ypr[1];
    yprpre[2]=ypr[2];
    Serial.println(ypr[0],DEC);
    delay(200);
    getSafeGyroPos();              
  }
 // Serial.println(yprpre[0],DEC);
 // Serial.println(ypr[0],DEC);
//  Serial.println("Calibration Finished!!!");  
}
 
 
void DESP_Gyro::init(){
  // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif


    // initialize device
 //   Serial.println(F("Initializing I2C devices..."));
    mpu->initialize();

    // verify connection
  //  Serial.println(F("Testing device connections..."));
    Serial.println(mpu->testConnection() ? F("6050 success") : F("6050 failed"));

    // wait for ready
  //  Serial.println(F("\nSend any character to begin DMP programming and demo: "));
  //  while (Serial.available() && Serial.read()); // empty buffer

    // load and configure the DMP
   // Serial.println(F("Initializing DMP..."));
    devStatus = mpu->dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu->setXGyroOffset(220);
    mpu->setYGyroOffset(76);
    mpu->setZGyroOffset(-85);
    mpu->setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
      //  Serial.println(F("Enabling DMP..."));
        mpu->setDMPEnabled(true);

        // enable Arduino interrupt detection
       // Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
       // attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu->getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
      //Serial.println(F("DMP ready! "));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu->dmpGetFIFOPacketSize();
  

        calibrateGyro(true);
        
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
//       Serial.print(F("DMP Initialization failed (code "));
//       Serial.print(devStatus);
//       Serial.println(F(")"));
    }  
}

void DESP_Gyro::goLeft(int deg){
   checkGyro(true);
   curbearing=ypr[0];
   targbearing=curbearing-deg;
   if (targbearing<-180)
    targbearing=360+targbearing;
   turning=true;
//   Serial.print("Turning 90 degrees left from ");
//   Serial.print(curbearing,DEC);
//   Serial.print(" to ");
//   Serial.println(targbearing,DEC);       
}

void DESP_Gyro::goRight(int deg){
   checkGyro(true);
   curbearing=ypr[0];
   targbearing=curbearing+deg;
   if (targbearing>180)
    targbearing=targbearing-360;
   turning=true;
  // Serial.print("Turning 90 degrees right from ");
  // Serial.print(curbearing,DEC);
  // Serial.print(" to ");
  // Serial.println(targbearing,DEC);       
}

int DESP_Gyro::getBearingDistance(float b1,float b2){
  int b1b;
  int b2b;
  int d3,g3,i3,j3,k3;
 
 
 
  //convert to 360
  if (b1<0) b1b=180+180+b1; else b1b=b1;
  if (b2<0) b2b=180+180+b2; else b2b=b2;  
 
  d3=b1b-b2b;
  g3=d3-360;
  i3=int(g3/360);
  if(abs(d3)<180) j3=d3;
    else j3=g3-i3*720;
 
  return j3;     // j3<0 then go right else go left
}

boolean DESP_Gyro::isTargetReached(int test){
  
	return abs(getBearingDistance(targbearing,curbearing))<=test;
}

void DESP_Gyro::setCurrentPosition(){
	curbearing=ypr[0];
}

int DESP_Gyro::getDistance(){
	return getBearingDistance(curbearing,targbearing);
}

void DESP_Gyro::targetReached(){
	turning=false;
}

void DESP_Gyro::getGyroSettings(boolean debug){
	redogyro=0;
  //checkGyro(debug);
  //RemDebug("GGS");
  do {  	   
       if (redogyro++<6){
         delay(5);//was 10
         checkGyro(debug);
         //checkGyro(true);
        
       } else break;               
  } while  ((ypr[0]==180) or (ypr[0]==0)); //sometimes is the wrong output so we recheck to be sure
  
  setCurrentPosition();
}

void DESP_Gyro::gotoBearing(float b){
	targbearing=b;
	turning=true;	
}

float DESP_Gyro::getSafeGyroPos(){
   float gyro2;
   int safecnt=0;
   // RemDebug("SGP");
    getGyroSettings(false);            
    
    do{
    	
     gyro2=curbearing; 
     getGyroSettings(false); 
    
     if (safecnt++>4) break;
    } while (abs(getBearingDistance(curbearing,gyro2))>3);//for safety we take measurements
 return curbearing;  
}
