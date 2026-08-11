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
    void on_pushButton_Connect_clicked();

    void on_lineEdit_ip_editingFinished();

    void on_lineEdit_port_editingFinished();

private:
    Ui::MainWindow *ui;
    ChamberWindow* chamber_ui = new ChamberWindow(this);
    QString default_ip = "127.0.0.1";//"192.168.1.12";
    uint16_t default_port = 22250; // 22252
    int connCntMax = 100;
private:
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H
