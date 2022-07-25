#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QPushButton>
#include <QTimer>
#include <QDateTime>


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


private slots:
    void readFromPort();

    void makePlot();
    void realtimeDataSlot();

    void sendMsgToDevice(QString message);
    QString updateMessageToSend();

    void on_connectButton_clicked();
    void on_searchButton_clicked();
    void on_disconnectButton_clicked();
    void on_pidUpdateButton_clicked();

    void on_pidAutoCheckbox_toggled(bool checked);

    void on_vstSlider_valueChanged(int value);

    void on_kpSlider_valueChanged(int value);

    void on_kiSlider_valueChanged(int value);

    void on_kdSlider_valueChanged(int value);

    void on_legendCheckBox_clicked(bool checked);

private:
    Ui::Widget *ui;

    QSerialPort * newDevice;

    //Writing serial variables
    QString updatePIDMessage;


    float a_setVelocity; // a_ means that this data became from Arduino serial port
    float a_measuredVelocity;

    float setVelocity; // set velocity from GUI slider


    float Kp;
    float Ki;
    float Kd;
    int tuningMethod;




    QTimer dataTimer;

    bool deviceConnected;
    int portNumbers;


    void addToLogs(QString message);

};
#endif // WIDGET_H
