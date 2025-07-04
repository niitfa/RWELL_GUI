#ifndef QGRAPH_H
#define QGRAPH_H

#include <QWidget>
#include <QtMath>
#include <QVector>
#include <QTime>
#include "qcustomplot.h"

namespace Ui {
class QGraph;
}

class QGraph : public QCustomPlot
{
    Q_OBJECT

public:
    explicit QGraph(QWidget *parent = nullptr);
    ~QGraph();

    void setYAxisRange(double yMix, double yMax);
    void setTAxisRange(double tMin, double tMax);

    void show();
    void update(int val);
    void resizeYAxis();
    void setEnabled(bool);
    double getTimeRange();
    double getYMax();
    double getYMin();
private:
    Ui::QGraph *ui;
    QTime timer;
    bool enabled = true;


    QVector<double> yVec;
    QVector<double> tVec;

    // t axis
    double tFilled = 0.75; // (0, 1]
    double tMin = 0;
    double tMax = 1;

    // y axis
    double yMin = 0;
    double yMax = 100;

private:
    void updateTimeVector(double t);
    bool isLimitTimeExceeded();
    void resetData();
    void setupGraph();
    void removeGraph();
    void replot();
};

#endif // QGRAPH_H
