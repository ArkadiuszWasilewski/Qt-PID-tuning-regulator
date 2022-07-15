#include <util/atomic.h>
#include "pins_define.h"
#include <Arduino.h>

class MotorControl
{
public:
	//pinPWM, pinDIR
	MotorControl(int, int);
	~MotorControl();

	void motorSetup();
	void moveForward(int valPWM); //test
	
	void motorPID(float Vst);  //regulator on Arduino side
	float measureVelocity();
	void setMotor(int valDIR, int valPWM , int pinPWM); //
	
	void addToBuffer(float val);

	int pos;
	float Kp;
	float Ki;
	float Kd;
	long currT;
	
	float v1Filt;

private:
	int _pinPWM, _pinDIR;		//motor pins
	float _setVelocity;
	
	int posPrev;
	
	long prevT;
	float deltaT;
	
	//Velocity and low-pass filter
	float velocity1;
	float v1;
	
	float v1Prev;

	//Regulator
	float u;
	float e;
	float eprev;
	float eintegral;
	float dedt;
	int dir;
	int pwr;
	

};

