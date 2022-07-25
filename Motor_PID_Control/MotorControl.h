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
	
	void motorPID(float Vst);  //regulator on Arduino side
	void changeParams(float setKp, float setKi, float setKd);
	float measureVelocity();
	void setMotor(int valDIR, int valPWM , int pinPWM); //
	

	int pos;
	float v1Filt;
	long currT;

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
	float _Kp;
	float _Ki;
	float _Kd;
	bool _reverseFlag;

	float u;
	float e;
	float eprev;
	float eintegral;
	float dedt;
	int dir;
	int pwr;
	

};

