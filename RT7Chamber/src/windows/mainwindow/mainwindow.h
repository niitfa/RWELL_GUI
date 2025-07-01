#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <stdint.h>
#include <memory>
#include "chamberwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_lineEdit_IP3_editingFinished();

    void on_lineEdit_IP2_editingFinished();

    void on_lineEdit_IP1_editingFinished();

    void on_lineEdit_IP0_editingFinished();

    void on_lineEdit_InputPort_editingFinished();

    void on_pushButton_Connect_clicked();

    void on_lineEdit_OutputPort_editingFinished();

private:
    Ui::MainWindow *ui;
    ChamberWindow* chamber_ui = new ChamberWindow(this);
    QVector<uint8_t> defaultIP = {169, 254, 206, 12};
    uint16_t defaultInputPort = 22252;
    uint16_t defaultOutputPort = 22251;
    int connCntMax = 100;

private:
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H
