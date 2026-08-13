#ifndef CHAMBERWINDOW_H
#define CHAMBERWINDOW_H

#include <QDialog>
#include <QTimer>
#include <QtMath>
#include <QFont>
#include <QString>
#include "qgraph.h"
#include "scansessionfile.h"
#include "averagecalculator.h"
#include "settingswindow.h"
#include "settings.h"

#include "rwell_client.h"

namespace Ui {
class ChamberWindow;
}

class ChamberWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ChamberWindow(QWidget *parent = nullptr);
    ~ChamberWindow() override;
    void show();

private:
    void setupClient();
    void setupSettingsButton();
    void setupConnectWidget();

    // connect widget callbacks
    void enableMainWindow();
    void disableMainWindow();
    // message received callback
    void updateWindowData();

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

    void on_lineEdit_BqPerCountMedium_editingFinished();

    void on_pushButton_settings_clicked();

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

    RWELLClient* client = nullptr;
    Ui::ChamberWindow *ui;
    SettingsWindow* settingsWindow = new SettingsWindow();
    QTimer* timer = nullptr;

    int id = 0;

    // Graph
    QGraph* graph = nullptr;
    double yGraphMaxRange = 90000; // was 9000000
    double yGraphMinRange = -90000;
    double tGraphRange = 20;
    const int maxVoltage = 500;

    // MBq per count
    double BqPerCountLow = 1;
    double BqPerCountMedium = 1;
    double BqPerCountHigh = 1;
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
    int8_t sensitivity = 0;
    int currDoseRate = 0;
    int currVoltage = 0; // debug !!!! 50
    int currPressure = 0;
    int currTemperature = 0;
    double currentActivity          = 0;
    double currentActivityWithNoise = 0;

    // average activity
    int averageDoseCountSaved = 0;
    int noiseCount_lowSense = 0;
    int noiseCount_mediumSense = 0;
    int noiseCount_highSense = 0;
    int noiseCount = 0;
private:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *) override;
};

#endif // CHAMBERWINDOW_H
