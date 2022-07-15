/*
 Name:		Motor_PID_Control.ino
 Created:	7/7/2022 5:06:44 PM
 Author:	twice
*/
#include "pins_define.h"
#include "MotorControl.h"
#include <util/atomic.h>
#include "SerialCom.h"

//Defining buffer size of received message
#define BUFFER_SZ 40


//Globals for ISR
static volatile uint32_t pos_i;

static volatile float receivedVset;
static volatile float receivedKp;
static volatile float receivedKi;
static volatile float receivedKd;
static volatile int receivedMethod;

/*static volatile*/ int pinAenc = pinA;
/*static volatile*/ int pinBenc = pinB;


MotorControl motor1(PWM, DIR);
SerialCom serial(9600);



void setup() {
	motor1.motorSetup();
	
	//Encoder Setup
	pinMode(pinAenc, INPUT_PULLUP);
	pinMode(pinBenc, INPUT_PULLUP);
	
	attachInterrupt(digitalPinToInterrupt(pinAenc), countA, FALLING);
	attachInterrupt(digitalPinToInterrupt(pinBenc), countB, FALLING);

	Serial.begin(9600);
}							


void loop() {
  
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		motor1.pos = pos_i;
	}

	//float Vst = 300 * (sin(motor1.currT / (1e6) * 0.5));
	//float Vst = 500;
	motor1.motorPID(receivedVset);


	static char buffer[BUFFER_SZ];
	static size_t lg = 0;

	while (Serial.available() > 0)
	{
		char c = Serial.read();
		if (c == '\r') {        // carriage return
			buffer[lg] = '\0';  // terminate the string
			parse(buffer);
			lg = 0;
		}
		else if (lg < BUFFER_SZ - 1) {
			buffer[lg++] = c;
		}

	}


	String bufferToSend;
	String comma = ",";
	//bufferToSend = (String)receivedVset + comma + (String)motor1.v1Filt + comma + (String)123;
	//bufferToSend = (String)motor1.Kp + comma + (String)motor1.Ki + comma + (String)motor1.Kd;

	bufferToSend = (String)receivedVset + comma + (String)receivedKp + comma + (String)receivedKi + comma + (String)receivedKd + comma + (String)receivedMethod;


	Serial.println(bufferToSend);
	delay(1);

}


void parse(char* buffer)
{
	char* s = strtok(buffer, ",");
	receivedVset = atoi(s);
	s = strtok(NULL, ",");
	receivedKp = atoi(s);
	s = strtok(NULL, ",");
	receivedKi = atoi(s);
	s = strtok(NULL, ",");
	receivedKd = atoi(s);
	s = strtok(NULL, ",");
	receivedMethod = atoi(s);

	Serial.print("Arduino: ");
	Serial.print(receivedVset);
	Serial.print(':');
	Serial.print(receivedKp);
	Serial.print(':');
	Serial.print(receivedKi);
	Serial.print(':');
	Serial.print(receivedKd);
	Serial.print(':');
	Serial.println(receivedMethod);



}

//ISR - Interrupt Service Routines
void countA()
{
	int increment;
	if (bitRead(PIND, pinAenc) != bitRead(PIND, pinBenc))
	{
		increment++;
	}
	else
	{
		increment--;
	}
	pos_i = pos_i + increment;
}

void countB()
{
	int increment;

	if (bitRead(PIND, pinAenc) == bitRead(PIND, pinBenc))
	{
		increment++;
	}
	else
	{
		increment--;
	}
	pos_i = pos_i + increment;
}



