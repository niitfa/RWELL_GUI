#ifndef CHAMBERWINDOW_H
#define CHAMBERWINDOW_H

#include <QDialog>
#include <QTimer>
#include <QtMath>
#include <QString>
#include "MessageReceiver.h"
#include "MessageTransmitter.h"

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
    void on_lineEdit_targetMeasNum_editingFinished();

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

private:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *) override;
};

#endif // CHAMBERWINDOW_H
