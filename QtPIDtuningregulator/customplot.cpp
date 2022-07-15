#include "customplot.h"
#include "ui_widget.h"

CustomPlot::CustomPlot(QWidget *parent)
    : QWidget(parent)
{

    CustomPlot::makePlot();
}

CustomPlot::~CustomPlot()
{}

void CustomPlot::makePlot()
{
    _customPlot->addGraph();
    _customPlot->graph(0)->setPen(QPen(QColor(40,110,255)));
    //add more signals later


    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");

    _customPlot->xAxis->setTicker(timeTicker);
    _customPlot->axisRect()->setupFullAxesBox();
    _customPlot->yAxis->setRange(-300, 300);

    // transfer ranges
    connect(_customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), _customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(_customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), _customPlot->yAxis2, SLOT(setRange(QCPRange)));

    //setup a timer
    connect(&_dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    _dataTimer.start(0);
    // 0 means - refresh as fast as possible
}

void CustomPlot::realtimeDataSlot()
{
    static QTime time(QTime::currentTime());
    // calculate two new data points:
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.005) // at most add point every 2 ms
    {
      // add data to lines:
      _customPlot->graph(0)->addData(key, 250);

      lastPointKey = key;
    }
    // make key axis range scroll with the data (at a constant range size of 8):
    _customPlot->xAxis->setRange(key, 8, Qt::AlignRight);
    _customPlot->replot();

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
