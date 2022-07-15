#include "MotorControl.h"


MotorControl::MotorControl(int pinPWM, int pinDIR)
{
	_pinPWM = pinPWM;
	_pinDIR = pinDIR;

	pos = 0;
	prevT = 0;
	posPrev = 0;

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

void MotorControl::moveForward(int valPWM)
{
	analogWrite(_pinPWM, valPWM);
	digitalWrite(_pinDIR, HIGH);
	//Serial.print("PWM: ");
	//Serial.println(valPWM);
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

void MotorControl::motorPID(float Vst)  
{
	MotorControl::measureVelocity();

	Kp = 2;
	//Kp = Serial.read(.....
	Ki = 0;
	Kd = 0;
	dir = 1;

	e = Vst - v1Filt;

	eintegral = eintegral + e * deltaT;
	dedt = (e - eprev) / deltaT;

	u = Kp * e + Ki * eintegral + Kd * dedt;

	if (u < 0)
	{
		dir = -1;//-1; //-1 if you want to faster adjust velocity, by changing direction, usually bad for motor because of fast changes
	}
	pwr = (int)fabs(u);
	if (pwr > 255)
	{
		pwr = 255;
	}
	if (pwr < 20) //with 20 pwm motor will not start, most likely
	{
		pwr = 20;
	}
	
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