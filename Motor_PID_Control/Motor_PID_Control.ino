/*
 Name:		Motor_PID_Control.ino
 Created:	7/7/2022 5:06:44 PM
 Author:	twice
*/
#include "pins_define.h"
#include "MotorControl.h"
#include <util/atomic.h>

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



void setup() {
	motor1.motorSetup();
	
	//Encoder
	pinMode(pinAenc, INPUT_PULLUP);
	pinMode(pinBenc, INPUT_PULLUP);
	
	//Interrupts
	attachInterrupt(digitalPinToInterrupt(pinAenc), countA, FALLING);
	attachInterrupt(digitalPinToInterrupt(pinBenc), countB, FALLING);

	//Serial communication init
	Serial.begin(BAUDRATE);
}							

void loop() {
  
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		motor1.pos = pos_i;
	}

	//float Vst = 300 * (sin(motor1.currT / (1e6) * 0.5));
	//float Vst = 300;
	float Vst = receivedVset;

	float Kp = receivedKp;
	float Ki = receivedKi;
	float Kd = receivedKd;
	int Method = receivedMethod;

	motor1.changeParams(Kp, Ki, Kd);
	motor1.motorPID(Vst);
	



	//Buffer received
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
	 
	//Buffer to send
	Serial.print(motor1.v1Filt);
	Serial.print(',');
	Serial.println(Vst);
	/*Serial.print(',');
	Serial.print(0);
	Serial.print(',');
	Serial.print(0);
	Serial.print(',');
	Serial.println(0);*/
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



