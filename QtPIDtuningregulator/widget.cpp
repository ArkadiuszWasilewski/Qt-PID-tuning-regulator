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

    velocity = 0;
    setVelocity = 0;
    value = 0;

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
        QStringList words={"0", "0", "0","0"};
        QString line = this->newDevice->readLine();
        //qDebug() << line;


        QString terminator = "\r";
        int pos = line.lastIndexOf(terminator);

        if(ui->receivedMessageCheckbox->isChecked())
        {
            this->addToLogs(line.left(pos));
        }


        words = line.left(pos).split(",");

        if(words.length()==1)
        {
            setVelocity = words[0].toDouble();
        }
        else if(words.length()==2)
        {
            setVelocity = words[0].toDouble();
            velocity = words[1].toDouble();
        }
        else if(words.length()==3)
        {
            setVelocity = words[0].toDouble();
            velocity = words[1].toDouble();
            value = words[2].toDouble();
        }
    }


    Widget::updateReceivedValue();
}

void Widget::makePlot()
{
    ui->customPlot->addGraph(); // blue line
    ui->customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    ui->customPlot->addGraph(); // red line
    ui->customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));
    ui->customPlot->addGraph(); // yellow line
    ui->customPlot->graph(2)->setPen(QPen(QColor(255, 210, 40)));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->customPlot->xAxis->setTicker(timeTicker);
    ui->customPlot->axisRect()->setupFullAxesBox();
    ui->customPlot->yAxis->setRange(-500.0, 500.0);

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
      //ui->customPlot->graph(0)->addData(key, value);
      ui->customPlot->graph(1)->addData(key, velocity);
      ui->customPlot->graph(2)->addData(key, setVelocity);

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


void Widget::updateReceivedValue()
{

    ui->label->setText(QString::number(setVelocity));
    ui->label_2->setText(QString::number(velocity));
    ui->label_3->setText(QString::number(value));
    //qDebug()<<"Set vel: "<<setVelocity;
    //qDebug()<<"Velocity:"<<velocity;
    //qDebug()<< "Value:" << value;
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
   QString updatePIDMessage;
   QString comma = ",";
   QString terminator = "\r";
   QString Vst = ui->vstValueLabel->text();
   QString Kp = ui->kpValueLabel->text();
   QString Ki = ui->kiValueLabel->text();
   QString Kd = ui->kdValueLabel->text();


   updatePIDMessage.reserve(Vst.length()+comma.length()+Kp.length()+comma.length()+Ki.length()+comma.length()+Kd.length()+comma.length()+terminator.length());
   updatePIDMessage.append(Vst);
   updatePIDMessage.append(comma);
   updatePIDMessage.append(Kp);
   updatePIDMessage.append(comma);
   updatePIDMessage.append(Ki);
   updatePIDMessage.append(comma);
   updatePIDMessage.append(Kd);
   updatePIDMessage.append(comma);
   updatePIDMessage.append(terminator);



   Widget::sendMsgToDevice(updatePIDMessage);
   qDebug() << "updatePIDMessage = " << updatePIDMessage;

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

}
