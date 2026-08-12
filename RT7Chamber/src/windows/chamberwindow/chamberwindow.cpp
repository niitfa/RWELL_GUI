#include "chamberwindow.h"
#include "ui_chamberwindow.h"
#include <QPalette>
#include <iostream>
#include <QtMath>

// 1) ChamberWindow::ChamberWindow(...) int interval = 10
// 2) ChamberWindow::update(...) if(...)
// 3) ChamberWindow::update(...) emul values
// 4) ChamberWindow::update(...) id++ emul


// проверить
// 1) переключение диапазонов, переключение коэффициентов

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

    // MCU connection
    this->client = new RWELLClient("127.0.0.1", 22250);

    // Init graph
    this->graph = new QGraph(ui->widget_graph);
    this->graph->setTAxisRange(0, static_cast<double>(this->tGraphRange));
    this->graph->setYAxisRange(this->yGraphMinRange, this->yGraphMaxRange);

    // line edits
    ui->lineEdit_graphVerticalMax->setText(QString::number(this->yGraphMaxRange));
    ui->lineEdit_graphVerticalMin->setText(QString::number(this->yGraphMinRange));
    ui->lineEdit_graphHorizontalRange->setText(QString::number(this->tGraphRange));
    ui->lineEdit_BqPerCountLow->setText(QString::number(this->BqPerCountLow)); //  MBq per count!!!
    ui->lineEdit_BqPerCountMedium->setText(QString::number(this->BqPerCountMedium)); //  MBq per count!!!
    ui->lineEdit_BqPerCountHigh->setText(QString::number(this->BqPerCountHigh)); //  MBq per count!!!

    // voltage menu widget
    ui->widget_voltageMenu->setClient(client);
    // activity menu widget
    ui->widget_activityMenu->hide(); // hide widget
    ui->verticalSpacer_activity->changeSize(0,0); // hide widget
    ui->widget_activityMenu->setClient(client);
    // polarity menu widget
    ui->widget_polarityMenu->hide(); // hide widget
    ui->verticalSpacer_polarity->changeSize(0,0); // hide widget
    ui->widget_polarityMenu->setClient(client);
    // sensivity menu widget
    ui->widget_sensivityMenu->setClient(client);
    // save to file widget
    ui->widget_fileMenu->setSession(&this->session);

    // start graph button
    this->graphStarted = 1;
    this->graphButtonStartText = "Возобновить";
    this->graphButtonStopText = "Остановить";
    this->setStopStyle(ui->pushButton_startGraph, graphButtonStopText);

    // noise measurement button
    this->noiseMeasurementStarted = 0;
    this->noiseButtonStartText = "Изм. фона: старт";
    this->noiseButtonStopText = "Изм. фона: стоп";
    this->setStartStyle(ui->pushButton_noiseMeasure, noiseButtonStartText);

    ui->pushButton_noiseReset->setText("Сбросить фон");

    // value widgets
    ui->widget_currentActivity->setHeadText("Активность (без фона), МБк:");
    ui->widget_currentActivityWithNoise->setHeadText("Активность (с фоном), МБк:");
    ui->widget_noiseLowSense->setHeadText("Фон (низкая чувств.), МБк:");
    ui->widget_noiseMediumSense->setHeadText("Фон (средняя чувств.), МБк:");
    ui->widget_noiseHighSense->setHeadText("Фон (высокая чувств.), МБк:");
    ui->widget_voltage->setHeadText("Напряжение, В:");
    ui->widget_pressure->setHeadText("Давление, атм:");
    ui->widget_temperature->setHeadText("Темп., °C:");

    // buttons
    buttonsFont.setFamily("Inter");
    buttonsFont.setPixelSize(15);
    buttonsFont.setWeight(50);

    // check box
    ui->checkBox_noise->setFont(buttonsFont);
    ui->checkBox_noise->setText("Вычесть фон");


    // set fonts
    ui->pushButton_resetScales->setFont(buttonsFont);
    ui->pushButton_startGraph->setFont(buttonsFont);
    ui->pushButton_noiseMeasure->setFont(buttonsFont);
    ui->pushButton_noiseReset->setFont(buttonsFont);
    ui->label->setFont(buttonsFont);
    ui->label_2->setFont(buttonsFont);
    ui->label_16->setFont(buttonsFont);
    ui->label_19->setFont(buttonsFont);
    ui->label_3->setFont(buttonsFont);
    ui->label_4->setFont(buttonsFont);
    ui->lineEdit_graphHorizontalRange->setFont(buttonsFont);
    ui->lineEdit_graphVerticalMax->setFont(buttonsFont);
    ui->lineEdit_graphVerticalMin->setFont(buttonsFont);
    ui->lineEdit_BqPerCountLow->setFont(buttonsFont);
    ui->lineEdit_BqPerCountMedium->setFont(buttonsFont);
    ui->lineEdit_BqPerCountHigh->setFont(buttonsFont);

    // bottom label rwell
    QFont rwellFont;
    rwellFont.setFamily("Bounded");
    rwellFont.setPixelSize(40);
    rwellFont.setWeight(80);

    double k = 0.18;
    ui->label_textRWELL->setPixmap(QIcon(":/img/icon_rwell.png").pixmap(QSize(1178 * k, 163 * k)));
}

ChamberWindow::~ChamberWindow()
{
    delete ui;
    delete client;
    delete graph;
}

void ChamberWindow::connect(std::string ip, uint16_t port)
{
    if(this->client)
    {
        client->setIP(ip);
        client->setPort(port);
        client->setVerbose(1);
        client->start();
    }
}

void ChamberWindow::disconnect()
{
    if(this->client)
    {
        client->stop();
    }
}

bool ChamberWindow::isConnected()
{
    bool connected = false;
    if(this->client)
    {
        connected = client->isConnected();
    }
    return connected;
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
    if(this->id < client->getMessageNum()) // real
    //if(1) // emulator
    {
        id = client->getMessageNum();
        //id++;

           // real
        this->cyclesRemained = 0;
        this->currDoseRate = client->getADCValue();
        this->currVoltage = client->getHV();
        this->currPressure = client->getPressure();
        this->currTemperature = client->getTemperature();
        this->hvPolarity = 0;
        this->sensitivity = client->getBand();

            // emulator
        /*this->cyclesRemained = 33;
        //this->currDoseRate = 1000000 * (1 + qSin(static_cast<double>(id)/20.));
        this->currDoseRate = 7900000 + (rand() % 200000);
        this->currVoltage = ui->widget_voltageMenu->getInputVoltage(); // debug !!!! 50
        this->currPressure = 1190;
        this->hvPolarity = 0;
        this->sensitivity = 1; */

        // noise update
        this->noiseUpdate(currDoseRate);
        if(this->sensitivity == 0)  {
            this->noiseCount = this->noiseCount_highSense;
            this->BqPerCount = this->BqPerCountHigh;
        }
        if(this->sensitivity == 1)  {
            this->noiseCount = this->noiseCount_mediumSense;
            this->BqPerCount = this->BqPerCountMedium;
        }
        if(this->sensitivity == 2)  {
            this->noiseCount = this->noiseCount_lowSense;
            this->BqPerCount = this->BqPerCountLow;
        }



        // widget values
        this->currentActivity          = (currDoseRate - this->noiseCount) * this->BqPerCount * 1e-6;
        this->currentActivityWithNoise = (currDoseRate) * this->BqPerCount * 1e-6;

        ui->widget_activityMenu->setMeasuresCompleted(cyclesRemained);
        ui->widget_currentActivity->setValueText(QString::number(currentActivity, 'f', 0));
        ui->widget_currentActivityWithNoise->setValueText(QString::number(currentActivityWithNoise, 'f', 0));
        ui->widget_noiseHighSense->setValueText(
                    QString::number(this->noiseCount_highSense * this->BqPerCountHigh * 1e-6, 'f', 0)
                    );
        ui->widget_noiseMediumSense->setValueText(
                    QString::number(this->noiseCount_mediumSense * this->BqPerCountMedium * 1e-6, 'f', 0)
                    );
        ui->widget_noiseLowSense->setValueText(
                    QString::number(this->noiseCount_lowSense * this->BqPerCountLow * 1e-6, 'f', 0)
                    );
        ui->widget_voltage->setValueText(QString::number(currVoltage));
        ui->widget_pressure->setValueText(QString::number((currPressure) / 10000., 'f', 2) + " / 12.00");
        ui->widget_temperature->setValueText(QString::number((currTemperature) / 100., 'f', 2));
        ui->widget_polarityMenu->setPolarity(hvPolarity);
        ui->widget_sensivityMenu->setSensivity(sensitivity);

        // graph update
        if(this->graph)
        {
            // calc activity
            this->graph->QGraph::update(
                        //(currDoseRate - this->averageDoseCountSaved) * this->BqPerCount * 1e-6
                        (currDoseRate - this->noiseCount * ( ui->checkBox_noise->checkState() == Qt::CheckState::Checked )) * this->BqPerCount * 1e-6
                        );
        }
        // file update
        logFileUpdate();
    }

    // update switch voltage button state
    const int switchVoltageLimit = static_cast<int>(this->maxVoltage * 0.05);
    ui->widget_polarityMenu->enableWidget( qAbs(client->getHV()) <  (switchVoltageLimit) );
}

void ChamberWindow::on_pushButton_startGraph_clicked()
{
    if(this->graphStarted)
    {
        if(this->graph)
        {
            this->graph->setEnabled(false);
            this->setStartStyle(ui->pushButton_startGraph, graphButtonStartText);
        }
    }
    else
    {
        if(this->graph)
        {
            this->graph->setEnabled(true);
            this->setStopStyle(ui->pushButton_startGraph, graphButtonStopText);
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

void ChamberWindow::setStartStyle(QPushButton * button, QString text)
{
    QIcon icon(":/img/button_icon_start.png");
    int iconSize = 14;
    button->setIcon(icon);
    button->setIconSize(QSize(iconSize,iconSize));
    button->setFont(buttonsFont);
    button->setText(text);

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

void ChamberWindow::setStopStyle(QPushButton * button, QString text)
{
    QIcon icon(":/img/button_icon_pause.png");
    int iconSize = 24;
    button->setIcon(icon);
    button->setIconSize(QSize(iconSize,iconSize));
    button->setFont(buttonsFont);
    button->setText(text);

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

void ChamberWindow::noiseUpdate(int noiseCount)
{
    if(this->noiseMeasurementStarted)
    {
        this->averageCalulator.add(static_cast<double>(noiseCount));
        if(this->sensitivity == 0)
        {
            this->noiseCount_highSense = static_cast<int>(this->averageCalulator.getAverage());
        }
        if(this->sensitivity == 1)
        {
            this->noiseCount_mediumSense =static_cast<int>(this->averageCalulator.getAverage());
        }
        if(this->sensitivity == 2)
        {
            this->noiseCount_lowSense =static_cast<int>(this->averageCalulator.getAverage());
        }
    }
}

void ChamberWindow::logFileUpdate()
{
    auto session = ui->widget_fileMenu->getSession();
    session->setID(this->id);
    session->setNoiseMBq(this->noiseCount * this->BqPerCount * 1e-6);
    session->setBqPerCount(this->BqPerCount);
    session->setRawActivityCount(this->currDoseRate);
    session->setRawActivityMBq(this->currentActivityWithNoise);
    session->setNoiselessActivityMBq(this->currentActivity);
    session->setSensitivity(this->sensitivity);
    session->setVoltagePolarity(this->hvPolarity);
    session->setVoltage(this->currVoltage);
    session->setPressure(this->currPressure / 100.);
    ui->widget_fileMenu->update(this->id);
}

void ChamberWindow::on_pushButton_noiseMeasure_clicked()
{
    if(this->noiseMeasurementStarted)
    {
        // stop action
        this->ui->pushButton_noiseReset->setEnabled(1);
        // start style
        setStartStyle(ui->pushButton_noiseMeasure, noiseButtonStartText);
        this->noiseMeasurementStarted = 0;
    }
    else
    {
        // start action
        this->averageCalulator.reset();
        this->ui->pushButton_noiseReset->setDisabled(1);
        // stop style
        setStopStyle(ui->pushButton_noiseMeasure, noiseButtonStopText);
        this->noiseMeasurementStarted = 1;
    }

}

void ChamberWindow::on_pushButton_noiseReset_clicked()
{
    this->noiseCount_highSense = 0;
    this->noiseCount_lowSense = 0;
}

void ChamberWindow::on_lineEdit_BqPerCountLow_editingFinished()
{
    double val = ui->lineEdit_BqPerCountLow->text().toDouble();
    if(val != 0.)
    {
        this->BqPerCountLow = val;
    }
}

void ChamberWindow::on_lineEdit_BqPerCountHigh_editingFinished()
{
    double val = ui->lineEdit_BqPerCountHigh->text().toDouble();
    if(val != 0.)
    {
        this->BqPerCountHigh = val;
    }
}

void ChamberWindow::on_lineEdit_BqPerCountMedium_editingFinished()
{
    double val = ui->lineEdit_BqPerCountMedium->text().toDouble();
    if(val != 0.)
    {
        this->BqPerCountMedium = val;
    }
}
