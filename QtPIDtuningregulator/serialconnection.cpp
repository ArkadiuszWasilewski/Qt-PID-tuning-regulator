#include "serialconnection.h"
#include <QDebug>

SerialConnection::SerialConnection(QObject *parent)
    : QObject{parent}
{

        //QString portName =

        _serial.setBaudRate(QSerialPort::Baud9600); // temporarly, in default you need to pass value setted by user in combobox
        _serial.setPortName("COM4"); // import port name from combobox

        _serial.setDataBits(QSerialPort::Data8);
        _serial.setParity(QSerialPort::NoParity);
        _serial.setStopBits(QSerialPort::OneStop);
        _serial.setFlowControl(QSerialPort::NoFlowControl);

        connect(&_serial, &QSerialPort::readyRead, this, &SerialConnection::newData);
}

SerialConnection::~SerialConnection()
{
    closeConnection();
}

void SerialConnection::openConnection()
{
    if(!_serial.open(QIODevice::ReadWrite))
    {
        qDebug() << "Connection impossible";
    }
    else
    {
        qDebug() << "Connection ok.";
    }
}

void SerialConnection::closeConnection()
{
    _serial.close();
}

void SerialConnection::newData()
{
    emit gotNewData(_serial.readAll());
}

void SerialConnection::write(const char* messageToWrite)
{
    _serial.write(messageToWrite);
}

bool SerialConnection::isOpen()
{
    return _serial.isOpen();
}

bool SerialConnection::isWriteable()
{
    return _serial.isWritable();
}
