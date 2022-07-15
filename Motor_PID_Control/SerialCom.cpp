#include "SerialCom.h"


SerialCom::SerialCom(int baudrate)
{
	_baudrate = baudrate;

	//SerialCom::delimiter();
	//Serial.begin(_baudrate);
}

SerialCom::~SerialCom()
{}

void addToBuffer(float val)
{
	//bufferToSend = (String)Vst + comma + (String)motor1.v1Filt + comma + (String)123;
	//delay(1000);

}

void delimiterChar()
{
	//delimiter = ",";
}