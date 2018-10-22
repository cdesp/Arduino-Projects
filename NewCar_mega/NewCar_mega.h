#ifndef newcar_h
#define newcar_h


//robot_logic
int RECV_PIN = A5; //define input pin on Arduino for IR Receiver  was 3

//IR obstacle Sensor NOT USED any more
#define obstacleL 0 
#define obstacleR 0 

//BlackLine Sensor
#define BlackLine1 23  
#define BlackLine2 25  
#define BlackLine3 27  
#define BlackLine4 29  
#define BlackLine5 31 
#define frontSensor 33  

//for 28byj NOT USED
#define sensmotor1 22
#define sensmotor2 24
#define sensmotor3 26
#define sensmotor4 28

//motor pins
#define mot1p1 48
#define mot1p2 46
#define mot2p1 50
#define mot2p2 52

#define mot3p1 49
#define mot3p2 47
#define mot4p1 51
#define mot4p2 53

#define motEN 30

//speed encoder pin and interrupt no
#define speedencpin 19
//#define speedint 4

//Sonic Sensor
int trPin=5;//4;
int ecPin=4;//5;

//gyro
int maxdeviation=4;//when turning how close to target we consider ok (degrees)


#endif


