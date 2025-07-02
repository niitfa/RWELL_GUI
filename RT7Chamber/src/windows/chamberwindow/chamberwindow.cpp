#include "chamberwindow.h"
#include "ui_chamberwindow.h"
#include <QPalette>
#include <iostream>

ChamberWindow::ChamberWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChamberWindow)
{
    ui->setupUi(this);
    setWindowTitle("RT7 Chamber");

    //init timer
    timer = new QTimer( this );
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->setSingleShot(false);
    timer->setInterval(5);

    //set readonly
    QPalette greyPalette;
    greyPalette.setColor(QPalette::Base, QColor(235, 235, 235));

    QVector<QLineEdit*> lineEditsRO =
    {
        ui->lineEdit_averDR,
        ui->lineEdit_currDR,
        ui->lineEdit_currVolt,
        ui->lineEdit_currPressure,
        ui->lineEdit_remainedMeasNum
    };
    for(auto line : lineEditsRO)
    {
        line->setReadOnly(true);
        line->setPalette(greyPalette);
    }

    this->receiver = new MessageReceiver();
    this->transmitter = new MessageTransmitter();
}

ChamberWindow::~ChamberWindow()
{
    delete ui;
    delete receiver;
    delete transmitter;
}

void ChamberWindow::connect(std::string ip, uint16_t outputPort, uint16_t inputPort)
{
    if(this->receiver)
    {
        receiver->Connect(ip, outputPort);
    }
    if(this->transmitter)
    {
        transmitter->Connect(ip, inputPort);
    }
}

void ChamberWindow::disconnect()
{
    if(this->receiver)
    {
        receiver->Disconnect();
    }
    if(this->transmitter)
    {
        transmitter->Disconnect();
    }
}

bool ChamberWindow::isConnected()
{
    bool receverConnected = true;
    bool transmitterConnected = true;
    if(this->receiver)
    {
        receverConnected = receiver->IsConnected();
    }
    if(this->transmitter)
    {
        transmitterConnected = transmitter->ping();
    }

    return (receverConnected && transmitterConnected);
}

void ChamberWindow::show()
{
    this->QDialog::show();
    if(!timer->isActive())
    {
        timer->start();
    }

}

void ChamberWindow::closeEvent(QCloseEvent *event)
{
    QDialog::closeEvent(event);
    // my code
    this->ChamberWindow::disconnect();

}
void ChamberWindow::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    // my code

}

void ChamberWindow::update()
{
    if(this->id < receiver->GetMessageID())
    {
        id = receiver->GetMessageID();

        int currDoseRate = receiver->GetADCValue();
        int averDoseRate = receiver->GetADCAverageValue();
        int currVoltage = receiver->GetHVOut();
        int currPressure = receiver->GetPressurePa();

        ui->lineEdit_currDR->setText(QString::fromStdString(std::to_string(currDoseRate)));
        ui->lineEdit_averDR->setText(QString::fromStdString(std::to_string(averDoseRate)));
        ui->lineEdit_currVolt->setText(QString::fromStdString(std::to_string(currVoltage)));
        ui->lineEdit_currPressure->setText(QString::fromStdString(std::to_string(currPressure)));
    }
}

void ChamberWindow::on_lineEdit_targetMeasNum_editingFinished()
{

}
