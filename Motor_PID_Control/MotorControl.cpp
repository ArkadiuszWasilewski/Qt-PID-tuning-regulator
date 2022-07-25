#include "MotorControl.h"


MotorControl::MotorControl(int pinPWM, int pinDIR)
{
	_pinPWM = pinPWM;
	_pinDIR = pinDIR;
	_Kp = 1;
	_Ki = 0;
	_Kd = 0;

	pos = 0;
	prevT = 0;
	posPrev = 0;
	dir = 1;

	v1Filt = 0;
	v1Prev = 0;
}

MotorControl::~MotorControl()
{
	
}

void MotorControl::motorSetup()
{
	pinMode(_pinPWM, OUTPUT);
	pinMode(_pinDIR, OUTPUT);
}

float MotorControl::measureVelocity()
{
	currT = micros();
	deltaT = (float)(currT - prevT) / 1.0e6;
	
	velocity1 = (pos - posPrev) / deltaT;
	posPrev = pos;
	prevT = currT;

	//Compute count/s to RPM (encoder: 20 ticks per rotation, 40 ticks if 2 interrupts)
	v1 = velocity1 / 40 * 60;	

	//Low-pass filter, 25Hz cutoff
	v1Filt = 0.854 * v1Filt + 0.0728 * v1 + 0.0728 * v1Prev;
	v1Prev = v1;

	return v1Filt; //filtered RPM 
}

void MotorControl::changeParams(float setKp, float setKi, float setKd)
{
	_Kp = setKp;
	_Ki = setKi;
	_Kd = setKd;
}

void MotorControl::motorPID(float Vst)  
{
	MotorControl::measureVelocity();

	e = Vst - v1Filt;
	eintegral = eintegral + e * deltaT;
	dedt = (e - eprev) / deltaT;

	u = _Kp * e + _Ki * eintegral + _Kd * dedt;

	//TODO: Arek //Preventing to set opposite direction while error is less than?

	if (u < 0)
	{
		dir = -1;
	}
	else if (u > 0)
	{
		dir = 1;
	}

	pwr = (int)fabs(u);
	if (pwr > 255)
	{
		pwr = 255;
	}
	//if (pwr < 20) //with 20 pwm motor will not start, most likely
	//{
	//	pwr = 20;
	//}
	
	MotorControl::setMotor(dir, pwr, _pinPWM);
}

void MotorControl::setMotor(int valDIR, int valPWM, int pinPWM)
{
	analogWrite(pinPWM, valPWM);
	if (valDIR == 1)
	{
		digitalWrite(_pinDIR, HIGH);
	}
	else if (valDIR == -1)
	{
		digitalWrite(_pinDIR, LOW);
	}
	else
	{
		digitalWrite(_pinDIR, LOW);
		analogWrite(_pinPWM, 0);
	}
}