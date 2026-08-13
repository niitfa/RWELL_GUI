#include "chamberwindow.h"
#include "ui_chamberwindow.h"
#include <QPalette>
#include <iostream>
#include <QtMath>
#include "settings.h"

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

    setupClient();
    setupSettingsButton();
    setupConnectWidget();

    BqPerCountLow = Settings::get(Settings::Key::BqPerCountLow, Settings::Section::Main).toString().toDouble();
    BqPerCountMedium = Settings::get(Settings::Key::BqPerCountMedium, Settings::Section::Main).toString().toDouble();
    BqPerCountHigh = Settings::get(Settings::Key::BqPerCountMedium, Settings::Section::Main).toString().toDouble();

    //init timer
    timer = new QTimer( this );
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->setSingleShot(false);
    timer->setInterval(50);

    //set readonly
    QPalette greyPalette;
    greyPalette.setColor(QPalette::Base, QColor(235, 235, 235));

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

    disableMainWindow();
}

ChamberWindow::~ChamberWindow()
{
    delete ui;
    delete client;
    delete graph;
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

void ChamberWindow::setupClient()
{
    std::string ip = Settings::get(Settings::Key::IP, Settings::Section::Main).toString().toStdString();
    uint16_t port = static_cast<uint16_t>(Settings::get(Settings::Key::Port, Settings::Section::Main).toInt());

    client = new RWELLClient(ip, port);
    client->setIP(ip);
    client->setPort(port);
    client->setVerbose(true);
    std::function<void()> msgRecvCallback(std::bind(&ChamberWindow::updateWindowData, this));
    client->setMessageReceivedCallback(msgRecvCallback);
}

void ChamberWindow::setupSettingsButton()
{
    QPushButton* button = ui->pushButton_settings;
    button->setText("");
    button->setStyleSheet("* { background-color: rgb(220,220,220) }");

    /* Icon */
    double iconScaleFactor = 0.70;
    QSize size = button->size();
    size.setWidth(static_cast<int>(size.width() * iconScaleFactor));
    size.setHeight(static_cast<int>(size.height() * iconScaleFactor));
    button->setIcon(QIcon(":/img/button_icon_settings.png"));
    button->setIconSize(size);
}

void ChamberWindow::setupConnectWidget()
{
    ui->widget_connect->setTCPClient(this->client);
    std::function<void()> enableCallback(std::bind(&ChamberWindow::enableMainWindow, this));
    ui->widget_connect->setConnectionEstablishedCallback(enableCallback);
    std::function<void()> disableCallback(std::bind(&ChamberWindow::disableMainWindow, this));
    ui->widget_connect->setConnectionLostCallback(disableCallback);
}

void ChamberWindow::enableMainWindow()
{
    uint8_t en = 1;
    ui->pushButton_noiseReset->setEnabled(en);
    ui->pushButton_startGraph->setEnabled(en);
    ui->pushButton_resetScales->setEnabled(en);
    ui->pushButton_noiseMeasure->setEnabled(en);
    ui->widget_graph->setEnabled(en);
    ui->widget_voltage->setEnabled(en);
    ui->widget_fileMenu->setEnabled(en);
    ui->widget_pressure->setEnabled(en);
    ui->widget_temperature->setEnabled(en);
    ui->widget_voltageMenu->setEnabled(en);
    ui->widget_activityMenu->setEnabled(en);
    ui->widget_polarityMenu->setEnabled(en);
    ui->widget_noiseLowSense->setEnabled(en);
    ui->widget_sensivityMenu->setEnabled(en);
    ui->widget_noiseHighSense->setEnabled(en);
    ui->widget_currentActivity->setEnabled(en);
    ui->widget_noiseMediumSense->setEnabled(en);
    ui->widget_currentActivityWithNoise->setEnabled(en);
    ui->widget_graph->setEnabled(en);
    ui->checkBox_noise->setEnabled(en);

    timer->start();
}

void ChamberWindow::disableMainWindow()
{
    uint8_t en = 0;
    ui->pushButton_noiseReset->setEnabled(en);
    ui->pushButton_startGraph->setEnabled(en);
    ui->pushButton_resetScales->setEnabled(en);
    ui->pushButton_noiseMeasure->setEnabled(en);
    ui->widget_graph->setEnabled(en);
    ui->widget_voltage->setEnabled(en);
    ui->widget_fileMenu->setEnabled(en);
    ui->widget_pressure->setEnabled(en);
    ui->widget_temperature->setEnabled(en);
    ui->widget_voltageMenu->setEnabled(en);
    ui->widget_activityMenu->setEnabled(en);
    ui->widget_polarityMenu->setEnabled(en);
    ui->widget_noiseLowSense->setEnabled(en);
    ui->widget_sensivityMenu->setEnabled(en);
    ui->widget_noiseHighSense->setEnabled(en);
    ui->widget_currentActivity->setEnabled(en);
    ui->widget_noiseMediumSense->setEnabled(en);
    ui->widget_currentActivityWithNoise->setEnabled(en);
    ui->checkBox_noise->setEnabled(en);

    timer->stop();
}

void ChamberWindow::updateWindowData()
{
    if(client)
    {
        this->id = client->getMessageNum();

        this->currDoseRate = client->getADCValue();
        this->currentActivity          = (currDoseRate - this->noiseCount) * this->BqPerCount * 1e-6;
        this->currentActivityWithNoise = (currDoseRate) * this->BqPerCount * 1e-6;
        this->currVoltage = client->getHV();
        this->currPressure = client->getPressure();
        this->currTemperature = client->getTemperature();
        this->sensitivity = client->getBand();

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
        ui->widget_sensivityMenu->setSensivity(sensitivity);

        logFileUpdate();
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
    if(client && client->isConnected())
    {
        if(this->graph && this->graphStarted)
        {
            // calc activity
            this->graph->QGraph::update(
                        //(currDoseRate - this->averageDoseCountSaved) * this->BqPerCount * 1e-6
                        (currDoseRate - this->noiseCount * ( ui->checkBox_noise->checkState() == Qt::CheckState::Checked )) * this->BqPerCount * 1e-6
                        );
        }
    }
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
        Settings::set(Settings::Key::BqPerCountLow, Settings::Section::Main) =
                ui->lineEdit_BqPerCountLow->text();
        this->BqPerCountLow = val;
    }
}

void ChamberWindow::on_lineEdit_BqPerCountHigh_editingFinished()
{
    double val = ui->lineEdit_BqPerCountHigh->text().toDouble();
    if(val != 0.)
    {
        Settings::set(Settings::Key::BqPerCountHigh, Settings::Section::Main) =
                ui->lineEdit_BqPerCountHigh->text();
        this->BqPerCountHigh = val;
    }
}

void ChamberWindow::on_lineEdit_BqPerCountMedium_editingFinished()
{
    double val = ui->lineEdit_BqPerCountMedium->text().toDouble();
    if(val != 0.)
    {
        Settings::set(Settings::Key::BqPerCountMedium, Settings::Section::Main) =
                ui->lineEdit_BqPerCountMedium->text();
        this->BqPerCountMedium = val;
    }
}

void ChamberWindow::on_pushButton_settings_clicked()
{
    settingsWindow->show();
}
