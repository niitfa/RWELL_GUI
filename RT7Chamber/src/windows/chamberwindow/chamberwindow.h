#ifndef CHAMBERWINDOW_H
#define CHAMBERWINDOW_H

#include <QDialog>
#include <QTimer>
#include <QtMath>
#include <QString>
#include "qgraph.h"
#include "MessageReceiver.h"
#include "MessageTransmitter.h"
#include "scansessionfile.h"

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

    void on_pushButton_switchVoltPolarity_clicked();

    void on_pushButton_switchRange_clicked();

    void on_lineEdit_targetVolt_editingFinished();

    void on_pushButton_changeVoltage_clicked();

    void on_pushButton_startGraph_clicked();

    void on_pushButton_stopGraph_clicked();

    void on_lineEdit_graphHorizontalRange_editingFinished();

    void on_lineEdit_graphVerticalMin_editingFinished();

    void on_lineEdit_graphVerticalMax_editingFinished();

    void on_pushButton_resetScales_clicked();

    void on_lineEdit_nAPerCount_editingFinished();

    void on_pushButton_compensateBG_clicked();

    void on_pushButton_resetBG_clicked();

    void on_pushButton_writeToFile_clicked();

    void on_lineEdit_writingPeriod_editingFinished();

private:

    MessageReceiver* receiver = nullptr;
    MessageTransmitter* transmitter = nullptr;
    Ui::ChamberWindow *ui;
    QTimer* timer = nullptr;

    // ranges
    QString qStrBroadRange = "широкий";
    QString qStrNarrowRange = "узкий";

    // polarities
    QString qStrPositivePolarity = "положит. (+)";
    QString qStrNegativePolarity = "отрицат. (-)";

    int id = 0;

    // Graph
    QGraph* graph = nullptr;
    double yGraphMaxRange = 9000000;
    double yGraphMinRange = -9000000;
    double tGraphRange = 20;
    const int maxVoltage = 500;

    // write to file
    bool writingToFileStarted = 0;
    int pointIndex = 0;
    int fileUpdatePeriod = 20 * 60;
    ScanSessionFile session;

private:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *) override;
};

#endif // CHAMBERWINDOW_H
