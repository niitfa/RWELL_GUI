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

    // set initial values
    ui->lineEdit_targetVolt->setText("0");
    ui->lineEdit_targetMeasNum->setText("100");

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
        transmitterConnected = transmitter->IsConnected();
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

        int cyclesRemained = receiver->GetMeasurementTime();
        int currDoseRate = receiver->GetADCValue();
        int averDoseRate = receiver->GetADCAverageValue();
        int currVoltage = receiver->GetHVOut();
        int currPressure = receiver->GetPressurePa();
        int8_t hvPolarity = receiver->GetHVPolarity();
        int8_t range = receiver->GetRange();

        ui->lineEdit_remainedMeasNum->setText(QString::fromStdString(std::to_string(cyclesRemained)));
        ui->lineEdit_currDR->setText(QString::fromStdString(std::to_string(currDoseRate)));
        ui->lineEdit_averDR->setText(QString::fromStdString(std::to_string(averDoseRate)));
        ui->lineEdit_currVolt->setText(QString::fromStdString(std::to_string(currVoltage)));
        ui->lineEdit_currPressure->setText(QString::fromStdString(std::to_string(currPressure)));

        if(!hvPolarity)  { ui->label_voltPolarity->setText(this->qStrPositivePolarity); }
        if(hvPolarity) { ui->label_voltPolarity->setText(this->qStrNegativePolarity); }

        if(!range) { ui->label_range->setText(this->qStrBroadRange); }
        if(range) { ui->label_range->setText(this->qStrNarrowRange); }
    }
}

void ChamberWindow::on_lineEdit_targetMeasNum_editingFinished()
{

}

void ChamberWindow::on_pushButton_startMeasure_clicked()
{
    this->ChamberWindow::on_pushButton_resetMeasure_clicked();
    if(this->transmitter)
    {
        int cycles = ui->lineEdit_targetMeasNum->text().toInt();
        transmitter->startMeasurement(cycles);
    }
}

void ChamberWindow::on_pushButton_resetMeasure_clicked()
{
    if(this->transmitter)
    {
        transmitter->resetMeasurement();
    }
}

void ChamberWindow::on_pushButton_switchVoltPolarity_clicked()
{
    if(this->transmitter && this->receiver)
    {
         if(receiver->GetHVPolarity())
         {
             this->transmitter->setPositiveVoltage();
         }
         else
         {
             this->transmitter->setNegativeVoltage();
         }
    }
}

void ChamberWindow::on_pushButton_switchRange_clicked()
{
    if(this->transmitter && this->receiver)
    {
         if(receiver->GetRange())
         {
             this->transmitter->setBroadRange();
         }
         else
         {
             this->transmitter->setNarrowRange();
         }
    }
}

void ChamberWindow::on_lineEdit_targetVolt_editingFinished()
{
    int targetVoltage = ui->lineEdit_targetVolt->text().toInt();
    if(targetVoltage < 0 || targetVoltage > 500)
    {
        ui->lineEdit_targetVolt->setText("0");
    }
}

void ChamberWindow::on_pushButton_changeVoltage_clicked()
{
    int targetVoltage = ui->lineEdit_targetVolt->text().toInt();
    if(this->transmitter)
    {
        this->transmitter->setVoltageValue(static_cast<uint16_t>(targetVoltage));
    }
}
