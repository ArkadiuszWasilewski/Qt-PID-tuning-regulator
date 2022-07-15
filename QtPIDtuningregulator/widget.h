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
    void updateReceivedValue();
    void realtimeDataSlot();

    void sendMsgToDevice(QString message);

    void on_connectButton_clicked();
    void on_searchButton_clicked();
    void on_disconnectButton_clicked();
    void on_pidUpdateButton_clicked();

    void on_pidAutoCheckbox_toggled(bool checked);

private:
    Ui::Widget *ui;

    QSerialPort * newDevice;

    //Reading serial variables
    QByteArray serialData;
    QString serialBuffer;

    //Writing serial variables
    QString updatePIDMessage;



    double velocity;
    double setVelocity;
    double value;

    QTimer dataTimer;

    bool deviceConnected;
    int portNumbers;


    void addToLogs(QString message);


};
#endif // WIDGET_H
