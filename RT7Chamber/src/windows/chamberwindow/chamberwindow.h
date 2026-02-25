#ifndef CHAMBERWINDOW_H
#define CHAMBERWINDOW_H

#include <QDialog>
#include <QTimer>
#include <QtMath>
#include <QFont>
#include <QString>
#include "qgraph.h"
#include "MessageReceiver.h"
#include "MessageTransmitter.h"
#include "scansessionfile.h"
#include "averagecalculator.h"

namespace Ui {
class ChamberWindow;
}

class ChamberWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ChamberWindow(QWidget *parent = nullptr);
    ~ChamberWindow() override;

    void connect(std::string ip, uint16_t outputPort, uint16_t inputPort);
    void disconnect();
    bool isConnected();
    void show();

private slots:
    void update();

    void on_pushButton_startGraph_clicked();

    void on_lineEdit_graphHorizontalRange_editingFinished();

    void on_lineEdit_graphVerticalMin_editingFinished();

    void on_lineEdit_graphVerticalMax_editingFinished();

    void on_pushButton_resetScales_clicked();

    void on_pushButton_noiseMeasure_clicked();

    void on_pushButton_noiseReset_clicked();

    void on_lineEdit_BqPerCountLow_editingFinished();

    void on_lineEdit_BqPerCountHigh_editingFinished();

private:
    void setStartStyle(QPushButton*, QString);
    void setStopStyle(QPushButton*, QString);

    // buttons texts
    QString graphButtonStartText; // graph
    QString graphButtonStopText;
    QString noiseButtonStartText; // noise
    QString noiseButtonStopText;

    void noiseUpdate(int noiseCount);
    void logFileUpdate();
private:

    MessageReceiver* receiver = nullptr;
    MessageTransmitter* transmitter = nullptr;
    Ui::ChamberWindow *ui;
    QTimer* timer = nullptr;

    int id = 0;

    // Graph
    QGraph* graph = nullptr;
    double yGraphMaxRange = 90000; // was 9000000
    double yGraphMinRange = -90000;
    double tGraphRange = 20;
    const int maxVoltage = 500;

    // MBq per count
    double BqPerCountLow = 13000;
    double BqPerCountHigh = 130;
    double BqPerCount = BqPerCountLow;

    // write to file
    ScanSessionFile session;

    // average calculator
    AverageCalculator averageCalulator;

    // start graph button state
    bool graphStarted = 1;
    bool noiseMeasurementStarted = 0;

    // font
    QFont buttonsFont;

    // received data
    uint8_t sensitivity = 0;
    uint8_t hvPolarity = 0;
    int cyclesRemained = 0;
    int currDoseRate = 0;
    int currVoltage = 0; // debug !!!! 50
    int currPressure = 0;
    double currentActivity          = 0;
    double currentActivityWithNoise = 0;

    // average activity
    int averageDoseCountSaved = 0;
    int noiseCount_lowSense = 0;
    int noiseCount_highSense = 0;
    int noiseCount = 0;
private:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *) override;
};

#endif // CHAMBERWINDOW_H
