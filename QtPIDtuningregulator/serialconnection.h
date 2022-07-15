#ifndef SERIALCONNECTION_H
#define SERIALCONNECTION_H

#include <QObject>
#include <QSerialPort>

class SerialConnection : public QObject
{
    Q_OBJECT
public:
    explicit SerialConnection(QObject *parent = nullptr);
    ~SerialConnection();

    void openConnection();
    void closeConnection();
    void write(const char* messageToWrite);
    bool isOpen();
    bool isWriteable();

private slots:
    void newData();

signals:
    void gotNewData(QByteArray);

private:
    QSerialPort _serial;


};

#endif // SERIALCONNECTION_H
