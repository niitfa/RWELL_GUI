#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "string_validator.h"
#include <string>
#include <iostream>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //setSize(350, 250);
    setGeometry(100, 100 , 400, 250);

    setWindowTitle("RT7 Chamber");
    // hide diargam
    //diagram->hide();

    // fill combo box
    ui->comboBox_TypeSelect->addItem("Колодезная камера");

    // fill ip and port with default values
    ui->lineEdit_IP3->setText(
                StringValidator::ValidateIPByte(
                    QString::fromStdString(std::to_string(defaultIP[3])))
                );
    ui->lineEdit_IP2->setText(
                StringValidator::ValidateIPByte(
                    QString::fromStdString(std::to_string(defaultIP[2])))
                );
    ui->lineEdit_IP1->setText(
                StringValidator::ValidateIPByte(
                    QString::fromStdString(std::to_string(defaultIP[1])))
                );
    ui->lineEdit_IP0->setText(
                StringValidator::ValidateIPByte(
                    QString::fromStdString(std::to_string(defaultIP[0])))
                );
    ui->lineEdit_InputPort->setText(
                StringValidator::ValidatePort(
                    QString::fromStdString(std::to_string(defaultInputPort)))
                );

    ui->lineEdit_OutputPort->setText(
                StringValidator::ValidatePort(
                    QString::fromStdString(std::to_string(defaultOutputPort)))
                );
}

MainWindow::~MainWindow()
{
    delete ui;
    delete chamber_ui;
}

void MainWindow::on_lineEdit_IP3_editingFinished()
{
    ui->lineEdit_IP3->setText(
                StringValidator::ValidateIPByte(ui->lineEdit_IP3->text())
                );

}

void MainWindow::on_lineEdit_IP2_editingFinished()
{
    ui->lineEdit_IP2->setText(
                StringValidator::ValidateIPByte(ui->lineEdit_IP2->text())
                );

}

void MainWindow::on_lineEdit_IP1_editingFinished()
{
    ui->lineEdit_IP1->setText(
                StringValidator::ValidateIPByte(ui->lineEdit_IP1->text())
                );
}

void MainWindow::on_lineEdit_IP0_editingFinished()
{
    ui->lineEdit_IP0->setText(
                StringValidator::ValidateIPByte(ui->lineEdit_IP0->text())
                );
}

void MainWindow::on_lineEdit_InputPort_editingFinished()
{
    ui->lineEdit_InputPort->setText(
                StringValidator::ValidatePort(ui->lineEdit_InputPort->text())
                );
}

void MainWindow::on_lineEdit_OutputPort_editingFinished()
{
    ui->lineEdit_OutputPort->setText(
                StringValidator::ValidatePort(ui->lineEdit_OutputPort->text())
                );
}


void MainWindow::on_pushButton_Connect_clicked()
{
    //ui->pushButton_Connect->setDisabled(true);

    // make ip string
    std::string ip = ui->lineEdit_IP0->text().toStdString() + "."
            + ui->lineEdit_IP1->text().toStdString() + "."
            + ui->lineEdit_IP2->text().toStdString() + "."
            + ui->lineEdit_IP3->text().toStdString();
    uint16_t inputPort = static_cast<uint16_t>(ui->lineEdit_InputPort->text().toInt());
    uint16_t outputPort = static_cast<uint16_t>(ui->lineEdit_OutputPort->text().toInt());


    QString type = ui->comboBox_TypeSelect->currentText();
    // pass obj to new window
    /*if(type == "4x4")
    {

        // connect attempt and connection check
        diagram->connect(ip, port,
                        {0, 1, 2, 3},
                        {0, 1, 4, 5}
                         );
        for (int i = 0; i < this->connCntMax; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            if(diagram->isConnected())
            {
                diagram->show();
                break;
            }

            // last iteration
            if(i == this->connCntMax - 1)
            {
                diagram->disconnect();
                QMessageBox::critical(this, " ", "Ошибка подключения!", QMessageBox::Close);

            }
        }
        // connect attempt ends
        *
    } */

    // FOR CHAMBER

    if(type == "Колодезная камера")
    {
        chamber_ui->show();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->QMainWindow::closeEvent(event);
    //diagram->disconnect();
}
