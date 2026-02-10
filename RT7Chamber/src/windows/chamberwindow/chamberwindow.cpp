#include "chamberwindow.h"
#include "ui_chamberwindow.h"
#include <QPalette>
#include <iostream>
#include <QtMath>

// 1) ChamberWindow::ChamberWindow(...) int interval = 10
// 2) ChamberWindow::update(...) if(...)
// 3) ChamberWindow::update(...) emul values
// 3) ChamberWindow::update(...) id++ emul

ChamberWindow::ChamberWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChamberWindow)
{
    ui->setupUi(this);
    setWindowTitle("RWELL");

    //init timer
    int interval = 50; // was 10

    timer = new QTimer( this );
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->setSingleShot(false);
    timer->setInterval(interval);

    //set readonly
    QPalette greyPalette;
    greyPalette.setColor(QPalette::Base, QColor(235, 235, 235));

    // set initial values
    ui->lineEdit_targetVolt->setText(QString::number(0));
    //ui->lineEdit_targetMeasNum->setText(QString::number(100));

    // Init graph
    this->graph = new QGraph(ui->widget_graph);
    this->graph->setNanoamperPerCount(4e-8);
    this->graph->resetNoise();
    this->graph->setTAxisRange(0, static_cast<double>(this->tGraphRange));
    this->graph->setYAxisRange(this->yGraphMinRange, this->yGraphMaxRange);

    ui->lineEdit_graphVerticalMax->setText(QString::number(this->yGraphMaxRange));
    ui->lineEdit_graphVerticalMin->setText(QString::number(this->yGraphMinRange));
    ui->lineEdit_graphHorizontalRange->setText(QString::number(this->tGraphRange));
    ui->lineEdit_nAPerCount->setText(QString::number(this->graph->getNanoamperPerCount()));

    // MCU connection
    this->receiver = new MessageReceiver();
    this->transmitter = new MessageTransmitter();

    // write to file
    this->fileUpdatePeriod = 20 * 60;
    ui->lineEdit_writingPeriod->setText(QString::number(fileUpdatePeriod));


    // value widgets
    ui->widget_currentActivity->setHeadText("Активность, МБк:");
    ui->widget_averageActivity->setHeadText("Средняя активность, МБк:");
    ui->widget_current->setHeadText("Ток, нА:");
    ui->widget_noiseCurrent->setHeadText("Фоновый ток, нА:");
    ui->widget_voltage->setHeadText("Напряжение, В:");
    ui->widget_pressure->setHeadText("Давление, атм:");

}

ChamberWindow::~ChamberWindow()
{
    delete ui;
    delete receiver;
    delete transmitter;
    delete graph;
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
    this->graph->show();
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
    show();
}

void ChamberWindow::update()
{
    // update message from MCU
    //if(this->id < receiver->GetMessageID()) // real
    if(1) // emulator
    {
        //id = receiver->GetMessageID();
        id++;

           // real
        /*int cyclesRemained = receiver->GetMeasurementTime();
        int currDoseRate = receiver->GetADCValue();
        int averDoseRate = receiver->GetADCAverageValue();
        int currVoltage = receiver->GetHVOut();
        int currPressure = receiver->GetPressurePa();
        int8_t hvPolarity = receiver->GetHVPolarity();
        int8_t range = receiver->GetRange(); */

            // emulator
        int cyclesRemained = 33;
        int currDoseRate = 1000000 * (1 + qSin(static_cast<double>(id)/20.));
        int averDoseRate = 230444;
        int currVoltage = 50;
        int currPressure = 1197;
        int8_t hvPolarity = 0;
        int8_t range = 1;

        ui->widget_activityMenu->setMeasuresCompleted(cyclesRemained);
        ui->widget_currentActivity->setValueText(QString::number(currDoseRate));
        ui->widget_averageActivity->setValueText(QString::number(averDoseRate));
        ui->widget_voltage->setValueText(QString::number(currVoltage));
        ui->widget_noiseCurrent->setValueText(QString::number(this->graph->getNoiseCount() * this->graph->getNanoamperPerCount()));
        ui->widget_current->setValueText(QString::number(this->graph->back()));
        ui->widget_pressure->setValueText(QString::number(static_cast<float>(currPressure) / 100.));

        if(!hvPolarity)  { ui->label_voltPolarity->setText(this->qStrPositivePolarity); }
        if(hvPolarity) { ui->label_voltPolarity->setText(this->qStrNegativePolarity); }

        if(!range) { ui->label_range->setText(this->qStrBroadRange); }
        if(range) { ui->label_range->setText(this->qStrNarrowRange); }

        // graph update
        if(this->graph)
        {
            this->graph->QGraph::updateCount(currDoseRate);
        }

        // file update

        if(this->pointIndex < (this->id / fileUpdatePeriod))
        {
            this->pointIndex = this->id / fileUpdatePeriod;
            session.update({id, currDoseRate});
        }

        if(!(this->id % fileUpdatePeriod))
        {

            session.update({id, currDoseRate});
        }
    }

    // update switch voltage button state
    const int switchVoltageLimit = static_cast<int>(this->maxVoltage * 0.05);
    ui->pushButton_switchVoltPolarity->setEnabled( qAbs(receiver->GetHVOut()) <  (switchVoltageLimit) );
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
}

void ChamberWindow::on_pushButton_changeVoltage_clicked()
{
    int targetVoltage = ui->lineEdit_targetVolt->text().toInt();
    if(this->transmitter)
    {
        if(targetVoltage < 0 || targetVoltage > this->maxVoltage) // check range
        {
            // bad value
            QMessageBox::critical(this,
                                  "Ошибка!",
                                  "Допустимый диапазон:\nV мин. = 0 В\nV макс. = " + QString::number(this->maxVoltage) + " B"
                                  );
        }
        else
        {
            // good value
            this->transmitter->setVoltageValue(static_cast<uint16_t>(targetVoltage));
        }
    }
}

void ChamberWindow::on_pushButton_startGraph_clicked()
{
    if(this->graph) { this->graph->setEnabled(true); }
}

void ChamberWindow::on_pushButton_stopGraph_clicked()
{
    if(this->graph) { this->graph->setEnabled(false); }
}

void ChamberWindow::on_lineEdit_graphHorizontalRange_editingFinished()
{
    double tRange = ui->lineEdit_graphHorizontalRange->text().toDouble();
    if(tRange > 0)
    {
        this->graph->setEnabled(false);
        this->graph->setTAxisRange(0, tRange);
    }
    else
    {
         ui->lineEdit_graphHorizontalRange->setText(
                     QString::fromStdString(std::to_string(this->graph->getTimeRange()))
                     );
    }


}

void ChamberWindow::on_lineEdit_graphVerticalMin_editingFinished()
{
    double min = ui->lineEdit_graphVerticalMin->text().toDouble();
    double currMax = this->graph->getYMax();
    if(min < currMax)
    {
        this->graph->setYAxisRange(min, currMax);
    }
    else
    {
        int currMinInt = static_cast<int>(this->graph->getYMin());
        ui->lineEdit_graphVerticalMin->setText(
                    QString::fromStdString(std::to_string(currMinInt))
                    );
    }
}

void ChamberWindow::on_lineEdit_graphVerticalMax_editingFinished()
{
    double max = ui->lineEdit_graphVerticalMax->text().toDouble();
    double currMin = this->graph->getYMin();
    if(max > currMin)
    {
        this->graph->setYAxisRange(currMin, max);
    }
    else
    {
        int currMaxInt = static_cast<int>(this->graph->getYMax());
        ui->lineEdit_graphVerticalMax->setText(
                    QString::fromStdString(std::to_string(currMaxInt))
                    );
    }
}

void ChamberWindow::on_pushButton_resetScales_clicked()
{
    this->graph->setYAxisRange(this->yGraphMinRange, this->yGraphMaxRange);
    ui->lineEdit_graphVerticalMax->setText(QString::fromStdString(std::to_string(static_cast<int>(this->yGraphMaxRange))));
    ui->lineEdit_graphVerticalMin->setText(QString::fromStdString(std::to_string(static_cast<int>(this->yGraphMinRange))));
}

void ChamberWindow::on_lineEdit_nAPerCount_editingFinished()
{
    double val = ui->lineEdit_nAPerCount->text().toDouble();
    if(val != 0.)
    {
        this->graph->setNanoamperPerCount(val);
    }
}

void ChamberWindow::on_pushButton_compensateBG_clicked()
{
    this->graph->updateNoise();
}

void ChamberWindow::on_pushButton_resetBG_clicked()
{
    this->graph->resetNoise();
}

void ChamberWindow::on_pushButton_writeToFile_clicked()
{
    if(this->writingToFileStarted)
    {
        // action
        ui->lineEdit_writingPeriod->setEnabled(1);
        ui->pushButton_writeToFile->setText("Записать в файл");
        // log - saved
        session.stop();
        // end action
        this->writingToFileStarted = 0;
    }
    else
    {
        // action
        ui->lineEdit_writingPeriod->setDisabled(1);
        ui->pushButton_writeToFile->setText("Остановить запись");

        // create file
        if(session.start())
        {
            this->pointIndex = 0; // reset points counter
            this->writingToFileStarted = 1;
        }
    }

}

void ChamberWindow::on_lineEdit_writingPeriod_editingFinished()
{
    int value = ui->lineEdit_writingPeriod->text().toInt();
    if (value <= 0)
    {
        this->fileUpdatePeriod = 1;
        ui->lineEdit_writingPeriod->setText(QString::number(this->fileUpdatePeriod));
    }
    else
    {
        this->fileUpdatePeriod = value;
    }
}
