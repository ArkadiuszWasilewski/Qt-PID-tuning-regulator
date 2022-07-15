#include <Arduino.h>

class SerialCom
{
public:
	SerialCom(int);
	~SerialCom();
	void addToBuffer(float val);
	void delimiterChar();

	String bufferToSend;

private:
	int _baudrate;
	String delimiter;

};

