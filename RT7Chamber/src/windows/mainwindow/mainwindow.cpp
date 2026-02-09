#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "string_validator.h"
#include <string>
#include <iostream>

// 1 edit void MainWindow::on_pushButton_Connect_clicked() - emul
// 2 edit MainWindow::on_pushButton_Connect_clicked() ip string parsing

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("RWELL");

    // head icon
    int iconSize = 32;
    ui->label_headIcon->clear();
    ui->label_headIcon->setPixmap(QIcon(":/img/button_icon_settings.png").pixmap(iconSize, iconSize));

    // head text
    ui->label_headText->clear();
    ui->label_headText->setText("Камера и подключение");

    // border
    ui->frame_connect->setStyleSheet(
                " QFrame { background-color: white; } "
                //" QFrame { border-radius:20px; }"
                //" QFrame { border-style: outset; }"
                //" QFrame { border-width:1px; }"
                //" QFrame { border-color:rgb(0,150,150); } "
                );



    // fill combo box
    ui->comboBox_TypeSelect->addItem("Колодезная камера");

    ui->frame_connect->setLayout(ui->verticalLayout_main);




    // fill ip and port with default values
    /*ui->lineEdit_IP3->setText(
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
                ); */
}

MainWindow::~MainWindow()
{
    delete ui;
    delete chamber_ui;
}

/*void MainWindow::on_lineEdit_IP3_editingFinished()
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
} */

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
    /*std::string ip = ui->lineEdit_IP0->text().toStdString() + "."
            + ui->lineEdit_IP1->text().toStdString() + "."
            + ui->lineEdit_IP2->text().toStdString() + "."
            + ui->lineEdit_IP3->text().toStdString(); */

    std::string ip = "192.168.1.12";
    uint16_t inputPort = static_cast<uint16_t>(ui->lineEdit_InputPort->text().toInt());
    uint16_t outputPort = static_cast<uint16_t>(ui->lineEdit_OutputPort->text().toInt());




    QString type = ui->comboBox_TypeSelect->currentText();

    // FOR CHAMBER
    /*if(type == "Колодезная камера")
    {
         chamber_ui->connect(ip, outputPort, inputPort);
         for (int i = 0; i < this->connCntMax; ++i)
         {
             std::this_thread::sleep_for(std::chrono::milliseconds(10));
             if(chamber_ui->isConnected())
             {
                 chamber_ui->show();
                 break;
             }

             // last iteration
             if(i == this->connCntMax - 1)
             {
                 //chamber_ui->disconnect();
                 QMessageBox::critical(this, " ", "Ошибка подключения!", QMessageBox::Close);

             }
         }
    } */

    // emulator!!!!
    chamber_ui->show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->QMainWindow::closeEvent(event);
}
