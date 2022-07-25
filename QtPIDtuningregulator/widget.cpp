#include "widget.h"
#include "ui_widget.h"
#include "qcustomplot.h"
#include "customplot.h"
#include <QDebug>
#include <QString>




Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);


    for(int i=0; i<QSerialPortInfo::standardBaudRates().length(); i++)
    {
        ui->baudrateCombobox->addItem(QString::number(QSerialPortInfo::standardBaudRates()[i]));
    }

    a_measuredVelocity = 0;
    a_setVelocity = 0;
    setVelocity = 0;
    Kp = 0;
    Ki = 0;
    Kd = 0;
    tuningMethod = 0;

    this->newDevice = new QSerialPort(this);


    Widget::makePlot();

}

Widget::~Widget()
{
    if(newDevice->isOpen())
    {
        newDevice->close(); //    Close the serial port if it's open.
    }

    delete ui;
}

void Widget::readFromPort()
{

    while(this->newDevice->canReadLine())
    {
        QStringList words={"0", "0", "0", "0", "0"};
        QString line = this->newDevice->readLine();
        qDebug() << line;


        QString terminator = "\r";
        int pos = line.lastIndexOf(terminator);

        if(ui->receivedMessageCheckbox->isChecked())
        {
            this->addToLogs(line.left(pos));
        }


        words = line.left(pos).split(',');

         switch(words.length())
         {
             case 1:
             {
                a_measuredVelocity = words[0].toDouble();
                break;
             }
             case 2:
             {
                a_measuredVelocity = words[0].toDouble();
                a_setVelocity = words[1].toDouble();
                break;
             }
             case 3:
             {
                a_measuredVelocity = words[0].toDouble();
                a_setVelocity = words[1].toDouble();

                break;
             }
             case 4:
             {
                 a_measuredVelocity = words[0].toDouble();
                 a_setVelocity = words[1].toDouble();

                break;
             }
             case 5:
             {
                a_measuredVelocity = words[0].toDouble();
                a_setVelocity = words[1].toDouble();

                break;
             }
             default:
             {
                qDebug() << "Cannot read full data from serialport";
             }
         }

    }
}

void Widget::makePlot()
{
    QFont legendFont = font();
    legendFont.setPointSize(9);
    ui->customPlot->legend->setFont(legendFont);
    ui->customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignLeft);


    ui->customPlot->addGraph(); // blue line
    ui->customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    ui->customPlot->graph(0)->setName("Program set velocity");

    ui->customPlot->addGraph(); // red line
    ui->customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));
    ui->customPlot->graph(1)->setName("Measured velocity");

    ui->customPlot->addGraph(); // yellow line
    ui->customPlot->graph(2)->setPen(QPen(QColor(255, 210, 40)));
    ui->customPlot->graph(2)->setName("Actual set velocity in MCU");




    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->customPlot->xAxis->setTicker(timeTicker);
    ui->customPlot->axisRect()->setupFullAxesBox();
    ui->customPlot->yAxis->setRange(ui->vstSlider->minimum()-50, ui->vstSlider->maximum()+50);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer.start(0); // Interval 0 means to refresh as fast as possible
}

void Widget::realtimeDataSlot()
{
    static QTime time(QTime::currentTime());
    // calculate two new data points:
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.005) // at most add point every 2 ms
    {
      // add data to lines:
      ui->customPlot->graph(0)->addData(key, setVelocity);  // setVelocity from GUI
      ui->customPlot->graph(1)->addData(key, a_measuredVelocity); // measured velocity from Arduino
      ui->customPlot->graph(2)->addData(key, a_setVelocity); // Vst from Arduino

      lastPointKey = key;
    }
    // make key axis range scroll with the data (at a constant range size of 8):
    ui->customPlot->xAxis->setRange(key, 8, Qt::AlignRight);
    ui->customPlot->replot();

    // calculate frames per second:
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key-lastFpsKey > 2) // average fps over 2 seconds
    {
      lastFpsKey = key;
      frameCount = 0;
    }
}

void Widget::on_connectButton_clicked()
{
    if(ui->portComboBox->count() == 0)
    {
      this->addToLogs("No devices detected!");
      return;
    }

    QString portName = ui->portComboBox->currentText().split(" ").first();
    this->newDevice->setPortName(portName);

    if(!newDevice->isOpen())
    {
        if(newDevice->open(QSerialPort::ReadWrite))
        {
            this->newDevice->setBaudRate(ui->baudrateCombobox->currentText().toInt());
            this->newDevice->setDataBits(QSerialPort::Data8);
            this->newDevice->setFlowControl(QSerialPort::NoFlowControl);
            this->newDevice->setParity(QSerialPort::NoParity);
            this->newDevice->setStopBits(QSerialPort::OneStop);

            QObject::connect(this->newDevice, SIGNAL(readyRead()), this, SLOT(readFromPort()));
            //QObject::connect(newDevice, SIGNAL(readyRead()), this, SLOT(readSerial()));

            this->addToLogs("Serial port opened.");
        }
    }
    else
    {
        this->addToLogs("Port is already opened.");
    }
}

void Widget::on_disconnectButton_clicked()
{
    if(this->newDevice->isOpen())
    {
        this->newDevice->close();
        this->addToLogs("Connection closed.");
    }
    else
    {
        this->addToLogs("Port is not opened!");
    }
}

void Widget::on_searchButton_clicked()
{
    this->addToLogs("Searching for new devices...");
    QList<QSerialPortInfo> devices;
    devices = QSerialPortInfo::availablePorts();

    for(int i = 0; i < devices.count(); i ++)
    {
        ui->portComboBox->removeItem(0);
        ui->portComboBox->removeItem(i);
    }

    for(int i = 0; i < devices.count(); i++)
    {
        this->addToLogs("Device found: " + devices.at(i).portName() + " " + devices.at(i).description());
        ui->portComboBox->addItem(devices.at(i).portName() + " " + devices.at(i).description());
    }
}

void Widget::addToLogs(QString message)
{
    QString currentDateTime = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->logsTextEdit->append(currentDateTime + " " + message);
}

void Widget::sendMsgToDevice(QString message)
{
    if(this->newDevice->isOpen() && this->newDevice->isWritable())
    {
        this->addToLogs("Sending msg to device " + message + "\n");
        this->newDevice->write(message.toStdString().c_str());
        qDebug()<< message.toStdString().c_str();
    }
    else
    {
        this->addToLogs("Port isn't open. Cannot send msg!");
    }
}

void Widget::on_pidUpdateButton_clicked()
{
   Widget::sendMsgToDevice(updateMessageToSend());
}

QString Widget::updateMessageToSend()
{
    QString updatePIDMessage;
    QString comma = ",";
    QString terminator = "\r";
    QString strVst = QString::number(setVelocity);
    QString strKp = QString::number(Kp);
    QString strKi = QString::number(Ki);
    QString strKd = QString::number(Kd);
    QString strMethod = QString::number(tuningMethod);


    updatePIDMessage.reserve(strVst.length()+comma.length()
                             +strKp.length()+comma.length()
                             +strKi.length()+comma.length()
                             +strKd.length()+comma.length()
                             +strMethod.length()+comma.length()
                             +terminator.length());

    updatePIDMessage.append(strVst);
    updatePIDMessage.append(comma);
    updatePIDMessage.append(strKp);
    updatePIDMessage.append(comma);
    updatePIDMessage.append(strKi);
    updatePIDMessage.append(comma);
    updatePIDMessage.append(strKd);
    updatePIDMessage.append(comma);
    updatePIDMessage.append(strMethod);
    updatePIDMessage.append(comma);
    updatePIDMessage.append(terminator);

    return updatePIDMessage;
}


void Widget::on_pidAutoCheckbox_toggled(bool checked)
{
    if(checked)
    {
        ui->pidUpdateButton->setEnabled(false);
        addToLogs("Auto update mode");
    }
    else
    {
        ui->pidUpdateButton->setEnabled(true);\
        addToLogs("Manual update mode");
    }

    if(ui->pidAutoCheckbox->isChecked())
    {
        Widget::sendMsgToDevice(updateMessageToSend());
    }

}

void Widget::on_vstSlider_valueChanged(int value)
{
    setVelocity = value;
    ui->vstValueLabel->setText(QString::number(setVelocity));

    if(ui->pidAutoCheckbox->isChecked())
    {
        Widget::sendMsgToDevice(updateMessageToSend());
    }

}

void Widget::on_kpSlider_valueChanged(int value)
{
    Kp = (float)value/10;
    ui->kpValueLabel->setText(QString::number(Kp));

    if(ui->pidAutoCheckbox->isChecked())
    {
        Widget::sendMsgToDevice(updateMessageToSend());
    }
}


void Widget::on_kiSlider_valueChanged(int value)
{
    Ki = (float)value/10;
    ui->kiValueLabel->setText(QString::number(Ki));

    if(ui->pidAutoCheckbox->isChecked())
    {
        Widget::sendMsgToDevice(updateMessageToSend());
    }
}


void Widget::on_kdSlider_valueChanged(int value)
{
    Kd = (float)value/10;
    ui->kdValueLabel->setText(QString::number(Kd));

    if(ui->pidAutoCheckbox->isChecked())
    {
        Widget::sendMsgToDevice(updateMessageToSend());
    }
}


void Widget::on_legendCheckBox_clicked(bool checked)
{
    if(checked)
    {
        ui->customPlot->legend->setVisible(true);
        addToLogs("Show legend on graph.");
    }
    else
    {
        ui->customPlot->legend->setVisible(false);
        addToLogs("Hide legend on graph.");
    }
}

