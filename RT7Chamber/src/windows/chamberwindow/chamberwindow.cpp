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
    this->setWindowTitle("RWELL");

    //init timer
    int interval = 10; // was 10

    timer = new QTimer( this );
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->setSingleShot(false);
    timer->setInterval(interval);

    //set readonly
    QPalette greyPalette;
    greyPalette.setColor(QPalette::Base, QColor(235, 235, 235));

    // set initial values
    //ui->lineEdit_targetVolt->setText(QString::number(0));
    //ui->lineEdit_targetMeasNum->setText(QString::number(100));

    // MCU connection
    this->receiver = new MessageReceiver();
    this->transmitter = new MessageTransmitter();

    // Init graph
    this->graph = new QGraph(ui->widget_graph);
    this->graph->setNanoamperPerCount(this->kBqPerCount_coarse * 1e-6); // MBq per count!!
    this->graph->resetNoise();
    this->graph->setTAxisRange(0, static_cast<double>(this->tGraphRange));
    this->graph->setYAxisRange(this->yGraphMinRange, this->yGraphMaxRange);

    // line edits
    ui->lineEdit_graphVerticalMax->setText(QString::number(this->yGraphMaxRange));
    ui->lineEdit_graphVerticalMin->setText(QString::number(this->yGraphMinRange));
    ui->lineEdit_graphHorizontalRange->setText(QString::number(this->tGraphRange));
    //ui->lineEdit_nAPerCount->setText(QString::number(this->graph->getNanoamperPerCount()));
    ui->lineEdit_nAPerCount->setText(QString::number(this->graph->getNanoamperPerCount() * 1e+6)); //  MBq per count!!!

    // voltage menu widget
    ui->widget_voltageMenu->registerConnectors(this->receiver, this->transmitter);
    // activity menu widget
    ui->widget_activityMenu->registerConnectors(this->receiver, this->transmitter);
    // polarity menu widget
    ui->widget_polarityMenu->registerConnectors(this->receiver, this->transmitter);
    // sensivity menu widget
    ui->widget_sensivityMenu->registerConnectors(this->receiver, this->transmitter);
    // save to file widget
    ui->widget_fileMenu->setSession(&this->session);

    // start graph button
    this->graphStarted = 1;
    this->setStopStyle(ui->pushButton_startGraph);

    // value widgets
    ui->widget_currentActivity->setHeadText("Активность, МБк:");
    ui->widget_averageActivity->setHeadText("Средняя активность, МБк:");
    ui->widget_current->setHeadText("Ток, нА:");
    ui->widget_noiseCurrent->setHeadText("Фоновый ток, нА:");
    ui->widget_voltage->setHeadText("Напряжение, В:");
    ui->widget_pressure->setHeadText("Давление, атм:");

    // buttons
    buttonsFont.setFamily("Inter");
    buttonsFont.setPixelSize(15);
    buttonsFont.setWeight(50);

    // buttons
    ui->pushButton_compensateBG->hide();
    //ui->pushButton_compensateBG->setFont(buttonsFont);
    //ui->pushButton_compensateBG->setText("Вычесть фон");
    ui->pushButton_resetBG->hide();

    // check box
    ui->checkBox_noise->setFont(buttonsFont);
    ui->checkBox_noise->setText("Вычесть фон");

    // set fonts
    ui->pushButton_compensateBG->setFont(buttonsFont);
    ui->pushButton_resetBG->setFont(buttonsFont);
    ui->pushButton_resetScales->setFont(buttonsFont);
    ui->pushButton_startGraph->setFont(buttonsFont);
    ui->label->setFont(buttonsFont);
    ui->label_2->setFont(buttonsFont);
    ui->label_16->setFont(buttonsFont);
    ui->label_19->setFont(buttonsFont);
    ui->lineEdit_graphHorizontalRange->setFont(buttonsFont);
    ui->lineEdit_graphVerticalMax->setFont(buttonsFont);
    ui->lineEdit_graphVerticalMin->setFont(buttonsFont);
    ui->lineEdit_nAPerCount->setFont(buttonsFont);

    // bottom label rwell
    QFont rwellFont;
    rwellFont.setFamily("Bounded");
    rwellFont.setPixelSize(40);
    rwellFont.setWeight(80);

    //ui->label_textRWELL->setFont(rwellFont);
   // ui->label_textRWELL->setStyleSheet(
    //            "color: rgb(129,142,172);"
    //            );
   // ui->label_textRWELL->setText("RWELL");
    double k = 0.18;
    ui->label_textRWELL->setPixmap(QIcon(":/img/icon_rwell.png").pixmap(QSize(1178 * k, 163 * k)));
    //ui->label_textRWELL->hide();

    ui->label_iconRWELL->hide();

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

#include <iostream>
void ChamberWindow::update()
{
    // update message from MCU
    if(this->id < receiver->GetMessageID()) // real
    //if(1) // emulator
    {
        id = receiver->GetMessageID();
        //id++;

           // real
        int cyclesRemained = receiver->GetMeasurementTime();
        int currDoseRate = receiver->GetADCValue();
        int averDoseRate = receiver->GetADCAverageValue();
        int currVoltage = receiver->GetHVOut();
        int currPressure = receiver->GetPressurePa();
        int8_t hvPolarity = receiver->GetHVPolarity();
        int8_t range = receiver->GetRange();

            // emulator
        /*int cyclesRemained = 33;
        int currDoseRate = 1000000 * (1 + qSin(static_cast<double>(id)/20.));
        int averDoseRate = 230444;
        int currVoltage = ui->widget_voltageMenu->getInputVoltage();//->widget_voltage->; // debug !!!! 50
        int currPressure = 1190;
        int8_t hvPolarity = 0;
        int8_t range = 1; */

        ui->widget_activityMenu->setMeasuresCompleted(cyclesRemained);

        // curren activity field
        //QString currActivityStr = getSubstringNumber(currDoseRate, 6);
        //ui->widget_currentActivity->setValueText(currActivityStr);
        ui->widget_currentActivity->setValueText(
                    QString::number((currDoseRate - this->averageDoseCountSaved)* getBqPerCount() * 1e-6, 'f', 0)
                    );

        // average activity field
        //QString averActivityStr = getSubstringNumber(averDoseRate, 4);
        ui->widget_averageActivity->setValueText(QString::number(averDoseRate * getBqPerCount() * 1e-6, 'f', 0));

        // curr voltage minus sigh
        //hvPolarity ?  currVoltage = -currVoltage : 1;
        ui->widget_voltage->setValueText(QString::number(currVoltage));

        // noise current field
        //QString noiseCurrentStr = getSubstringNumber(this->graph->getNoiseCount() * this->kNanoAmperPerCount, 6);
        ui->widget_noiseCurrent->setValueText(
                    //QString::number(this->graph->getNoiseCount() * this->kNanoAmperPerCount, 'f', 6)
                    QString::number(this->averageDoseCountSaved * this->kNanoAmperPerCount, 'f', 6)
                    );

        // curr current field
        //QString currCurrentStr = getSubstringNumber(currDoseRate * this->kNanoAmperPerCount, 6);
        ui->widget_current->setValueText(
                    QString::number((currDoseRate - this->averageDoseCountSaved) * this->kNanoAmperPerCount, 'f', 6)
                    );

        ui->widget_pressure->setValueText(QString::number((currPressure) / 100., 'f', 2) + " / 12.00");
        ui->widget_polarityMenu->setPolarity(hvPolarity);
        ui->widget_sensivityMenu->setSensivity(range);

        // rescale y-axis
        if(range != this->prevRange)
        {
           this->graph->setNanoamperPerCount(this->getBqPerCount() * 1e-6); // MBq per count!!
           this->prevRange = range;
        }

        // graph update
        if(this->graph)
        {
            // calc activity
            this->graph->QGraph::updateCount(currDoseRate - this->averageDoseCountSaved);
        }
        // file update
        ui->widget_fileMenu->update(id, currDoseRate);
    }

    // update switch voltage button state
    const int switchVoltageLimit = static_cast<int>(this->maxVoltage * 0.05);
    ui->widget_polarityMenu->enableWidget( qAbs(receiver->GetHVOut()) <  (switchVoltageLimit) );
}

void ChamberWindow::on_pushButton_startGraph_clicked()
{
    if(this->graphStarted)
    {
        if(this->graph)
        {
            this->graph->setEnabled(false);
            this->setStartStyle(ui->pushButton_startGraph);
        }
    }
    else
    {
        if(this->graph)
        {
            this->graph->setEnabled(true);
            this->setStopStyle(ui->pushButton_startGraph);
        }
    }
    this->graphStarted = !this->graphStarted;
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
    // MBq!!!
    double val = ui->lineEdit_nAPerCount->text().toDouble();
    if(val != 0.)
    {
        this->kBqPerCount_coarse = val;
        this->graph->setNanoamperPerCount(getBqPerCount());
    }

}

void ChamberWindow::on_pushButton_compensateBG_clicked()
{
    this->graph->updateNoise();
    /*if(receiver)
    {
        this->averageDoseCountSaved = receiver->GetADCAverageValue();
    } */
}

void ChamberWindow::on_pushButton_resetBG_clicked()
{
    this->graph->resetNoise();
}

double ChamberWindow::getBqPerCount()
{
    if(receiver)
    {
        if(receiver->GetRange())
        {
            return this->kBqPerCount_coarse / this->kSense;
        }
        else
        {
            return this->kBqPerCount_coarse;
        }
    }
    return this->kBqPerCount_coarse;
}

void ChamberWindow::setStartStyle(QPushButton * button)
{
    QIcon icon(":/img/button_icon_start.png");
    int iconSize = 14;
    button->setIcon(icon);
    button->setIconSize(QSize(iconSize,iconSize));
    button->setFont(buttonsFont);
    button->setText("Возобновить");

    button->setStyleSheet(
                // unpressed
                "QPushButton { border-style: outset; }"
                "QPushButton { border-radius:5px; }"
                "QPushButton { border-width:1px; }"
                "QPushButton { border-color: rgb(50,100,210); }"
                "QPushButton { background-color: rgb(60,120,230); }"
                "QPushButton { color: white; }"
                // hover
                "QPushButton:hover { background-color: rgb(40,100,200);  }"
                // pressed
                "QPushButton:pressed { background-color: rgb(30,80,170);  }"
                ); // Start */
    button->setFocusPolicy( Qt::FocusPolicy::NoFocus );

}

void ChamberWindow::setStopStyle(QPushButton * button)
{
    QIcon icon(":/img/button_icon_pause.png");
    int iconSize = 24;
    button->setIcon(icon);
    button->setIconSize(QSize(iconSize,iconSize));
    button->setFont(buttonsFont);
    button->setText("Остановить");

    button->setStyleSheet(
                // unpressed
                "QPushButton { border-style: outset; }"
                "QPushButton { border-radius:5px; }"
                "QPushButton { border-width:1px; }"
                "QPushButton { border-color: rgb(50,100,210); }"
                "QPushButton { background-color: rgb(60,120,230); }"
                "QPushButton { color: white; }"
                // hover
                "QPushButton:hover { background-color: rgb(40,100,200);  }"
                // pressed
                "QPushButton:pressed { background-color: rgb(30,80,170);  }"
                ); // Start */
    button->setFocusPolicy( Qt::FocusPolicy::NoFocus );
}

void ChamberWindow::on_checkBox_noise_clicked()
{
    if(ui->checkBox_noise->checkState())
    {
        //std::cout << "check state: " << true << std::endl;
        if(receiver)
        {
            this->averageDoseCountSaved = receiver->GetADCAverageValue();

        }
    }
    else
    {
        //std::cout << "check state: " << false << std::endl;
         this->averageDoseCountSaved = 0;
    }
}
