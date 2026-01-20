#include "chamberwindow.h"
#include "ui_chamberwindow.h"
#include <QPalette>
#include <iostream>
#include <QtMath>

ChamberWindow::ChamberWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChamberWindow)
{
    ui->setupUi(this);
    setWindowTitle("RWELL");

    //init timer
    timer = new QTimer( this );
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->setSingleShot(false);
    timer->setInterval(20);

    //set readonly
    QPalette greyPalette;
    greyPalette.setColor(QPalette::Base, QColor(235, 235, 235));

    QVector<QLineEdit*> lineEditsRO =
    {
        ui->lineEdit_averDR,
        ui->lineEdit_currDR,
        ui->lineEdit_currVolt,
        ui->lineEdit_currPressure,
        ui->lineEdit_remainedMeasNum,
        ui->lineEdit_signalCurrent,
        ui->lineEdit_backgroundCurrent
    };
    for(auto line : lineEditsRO)
    {
        line->setReadOnly(true);
        line->setPalette(greyPalette);
    }

    // set initial values
    ui->lineEdit_targetVolt->setText(QString::number(0));
    ui->lineEdit_targetMeasNum->setText(QString::number(100));

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

        ui->lineEdit_remainedMeasNum->setText(QString::number(cyclesRemained));
        ui->lineEdit_currDR->setText(QString::number(currDoseRate));
        ui->lineEdit_averDR->setText(QString::number(averDoseRate));
        ui->lineEdit_currVolt->setText(QString::number(currVoltage));
        ui->lineEdit_currPressure->setText(QString::number(currPressure));
        //ui->lineEdit_signalCurrent->setText(QString::number(this->receiver->GetADCValue() * this->graph->getNanoamperPerCount()));
        ui->lineEdit_signalCurrent->setText(QString::number(this->graph->back()));
        ui->lineEdit_backgroundCurrent->setText(QString::number(this->graph->getNoiseCount() * this->graph->getNanoamperPerCount()));

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
        //int fileUpdatePeriod = ui->lineEdit_writingPeriod->text().toInt();
        if(!(this->id % fileUpdatePeriod))
        {
            session.update({id, currDoseRate});
        }
    }

    // update switch voltage button state
    const int switchVoltageLimit = static_cast<int>(this->maxVoltage * 0.05);
    ui->pushButton_switchVoltPolarity->setEnabled( qAbs(receiver->GetHVOut()) <  (switchVoltageLimit) );
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
