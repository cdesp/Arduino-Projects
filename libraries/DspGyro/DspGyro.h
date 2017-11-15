#ifndef DspGyro_H
 #define DspGyro_H


#include "Arduino.h"
#include "helper_3dmath.h"
//#include <..\MPU6050\helper_3dmath.h>

#define MPU6050_INCLUDE_DMP_MOTIONAPPS20
#include "MPU6050.h"
//#include <..\MPU6050\MPU6050.h>


//#define OUTPUT_READABLE_QUATERNION
#define OUTPUT_READABLE_YAWPITCHROLL
//#define OUTPUT_READABLE_REALACCEL
//#define OUTPUT_READABLE_WORLDACCEL
//#define OUTPUT_TEAPOT


typedef void (*remdebug) (String) ;

 
class DESP_Gyro
{
  private:
  // class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
	  MPU6050 *mpu;      
      
// MPU control/status vars
    bool dmpReady ;  // set true if DMP init was successful
    uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
    uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
    uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
    uint16_t fifoCount;     // count of all bytes currently in FIFO
    uint8_t fifoBuffer[64]; // FIFO storage buffer
    
// orientation/motion vars
    Quaternion q;           // [w, x, y, z]         quaternion container
    VectorInt16 aa;         // [x, y, z]            accel sensor measurements
    VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
    VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
    VectorFloat gravity;    // [x, y, z]            gravity vector
    float euler[3];         // [psi, theta, phi]    Euler angle container
    float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
    float yprpre[3];  

// packet structure for InvenSense teapot demo
    uint8_t teapotPacket[14];    
    int redogyro;  
    
    
  public:
  	remdebug RemDebug;
    float curbearing;
    float targbearing;
    boolean turning;

    DESP_Gyro(int addr);
    void checkGyro(boolean debug);
    void calibrateGyro(boolean debug);
    void init();
    void goLeft(int deg);
    void goRight(int deg);
    int getBearingDistance(float b1,float b2);
    int getDistance();
    boolean isTargetReached(int test);
    void targetReached();
    void setCurrentPosition();
    void getGyroSettings(boolean debug);
    float getSafeGyroPos();
    void gotoBearing(float b);
};




#endif
