#ifndef CUSTOMPLOT_H
#define CUSTOMPLOT_H

#include <QWidget>
#include <QTimer>
#include <QDateTime>
#include "qcustomplot.h"

class CustomPlot : public QWidget
{
    Q_OBJECT
public:
    explicit CustomPlot(QWidget *parent = nullptr);
    ~CustomPlot();
    void makePlot();

    void startPlotting();
    void realtimeDataSlot();


    void addInputSignal();

private slots:


signals:

private:
    QCustomPlot *_customPlot;
    QTimer _dataTimer;

};

#endif // CUSTOMPLOT_H
