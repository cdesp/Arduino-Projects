#ifndef DspGyro_H
 #define DspGyro_H

//My Wrapper for GY-273 HMC5883L with QMC chip

#include "Arduino.h"
#include <Wire.h>
#include <MechaQMC5883.h>

typedef void (*remdebug) (String) ;
 
class DESP_Gyro
{
  private:
	  MechaQMC5883 *qmc;
      
    int azimuth;    
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
