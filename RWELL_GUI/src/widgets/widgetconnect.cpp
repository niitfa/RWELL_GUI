#include "widgetconnect.h"
#include "ui_widgetconnect.h"
#include "mainwindow.h"
#include <QThread>
#include <settings.h>

WidgetConnect::WidgetConnect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetConnect)
{
    ui->setupUi(this);
    setupStatusLED();
    drawCurrentButtonStyle();
}

WidgetConnect::~WidgetConnect()
{
    delete timer;
    delete ui;
}

void WidgetConnect::setTCPClient(TCPClient *client)
{
    this->client = client;
}

void WidgetConnect::setConnectionEstablishedCallback(std::function<void ()>& f)
{
    this->connectionEstablishedCallback = f;
}

void WidgetConnect::setConnectionLostCallback(std::function<void ()>& f)
{
    this->connectionLostCallback = f;
}

void WidgetConnect::setupStatusLED()
{
    this->statusLED = new StatusLED(ui->radioButton_status, nullptr);
    this->statusLED->drawNoData();
}

void WidgetConnect::drawCurrentButtonStyle()
{
    QPushButton* button = ui->pushButton_connect;
    if(this->buttonConnectState)
    {
        button->setText("");
        button->setStyleSheet("* { background-color: rgb(220,220,220) }");
        drawButtonIcon();
    }
    else
    {
        button->setText("");
        button->setStyleSheet("* { background-color: rgb(170,170,170) }");
        drawButtonIcon();
    }
}

void WidgetConnect::drawButtonIcon()
{
    QPushButton* button = ui->pushButton_connect;
    double iconScaleFactor = 0.6;
    QSize size = button->size();
    size.setWidth(static_cast<int>(size.width() * iconScaleFactor));
    size.setHeight(static_cast<int>(size.height() * iconScaleFactor));
    button->setIcon(QIcon(":/img/button_icon_connect.png"));
    button->setIconSize(size);
}

void WidgetConnect::watchdogEnable()
{
    if(!this->timer)
    {
        this->timer = new QTimer();
        this->timer->setSingleShot(false);
        this->timer->setInterval(this->wdInterval_ms);
        QObject::connect(this->timer, SIGNAL(timeout()), this, SLOT(watchdogTick()));
    }
    this->wdState = WatchdogState::Init;
    this->timer->start();
}

void WidgetConnect::watchdogDisable()
{
    if(this->timer)
    {
        this->timer->stop();
    }
}

void WidgetConnect::watchdogTick()
{
    switch ( this->wdState )
    {
    case WatchdogState::Init:
        this->wdConnectionCounter = (this->wdConnectionTime_ms / this->wdInterval_ms) + 1;
        this->statusLED->drawProcess();
        this->client->start();
        this->wdState = WatchdogState::WaitingForConnection;
        break;
    case WatchdogState::WaitingForConnection:
        if(this->wdConnectionCounter--)
        {
            if(this->client && this->client->isConnected())
            {
                this->statusLED->drawReady();
                this->buttonConnectState = false;
                this->drawCurrentButtonStyle();
                this->connectionEstablishedCallback();
                this->wdState = WatchdogState::ConnectionEstablished;
            }
        }
        else
        {
            this->wdState = WatchdogState::ConnectionLost;
        }
        break;
    case WatchdogState::ConnectionEstablished:
        if(this->client && !this->client->isConnected())
        {
            this->wdState = WatchdogState::ConnectionLost;
        }
        break;
    case WatchdogState::ConnectionLost:
        this->statusLED->drawError();
        this->buttonConnectState = true;
        this->drawCurrentButtonStyle();
        this->client->stop();
        this->watchdogDisable();
        this->connectionLostCallback();
        break;
    case WatchdogState::DisconnectedByUser:
        this->statusLED->drawNoData();
        this->buttonConnectState = true;
        this->drawCurrentButtonStyle();
        this->client->stop();
        this->watchdogDisable();
        this->connectionLostCallback();
        break;
    }
}

void WidgetConnect::on_pushButton_connect_clicked()
{
    if(this->buttonConnectState)
    {
        if(this->client)
        {
           this->client->setIP(Settings::get(Settings::Key::IP, Settings::Section::Main).toString().toStdString());
           this->client->setPort(static_cast<uint16_t>(Settings::get(Settings::Key::Port, Settings::Section::Main).toInt()));
           watchdogEnable();
        }
    }
    else
    {
        if(this->client)
        {
            this->wdState = WatchdogState::DisconnectedByUser;
        }
    }
}
