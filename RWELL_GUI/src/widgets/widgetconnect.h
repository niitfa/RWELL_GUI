#ifndef WIDGETCONNECT_H
#define WIDGETCONNECT_H

#include <QWidget>
#include <QTimer>
#include <functional>
#include "tcp_client.h"
#include "statusled.h"

namespace Ui {
class WidgetConnect;
}

class WidgetConnect : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetConnect(QWidget *parent = nullptr);
    ~WidgetConnect();
    void setTCPClient(TCPClient*);
    void setConnectionEstablishedCallback(std::function<void()>&);
    void setConnectionLostCallback(std::function<void()>&);
private slots:
    void watchdogTick();
    void on_pushButton_connect_clicked();
private:
    void setupStatusLED();
    void setupConnectionWatchdog();
    void drawCurrentButtonStyle();
    void drawButtonIcon();
    bool waitForConnection(uint32_t msec);

    void watchdogEnable();
    void watchdogDisable();

private:
    Ui::WidgetConnect *ui;
    TCPClient* client = nullptr;
    StatusLED* statusLED = nullptr;
    bool buttonConnectState = true;

    // connection watchdog
    enum class WatchdogState
    {
        Init,
        WaitingForConnection,
        ConnectionEstablished,
        ConnectionLost,
        DisconnectedByUser
    };

    WatchdogState wdState = WatchdogState::Init;
    QTimer* timer = nullptr;
    const int wdInterval_ms = 500;
    const int wdConnectionTime_ms = 2000;
    int wdConnectionCounter = 0;

    std::function<void(void)> connectionEstablishedCallback = [](void) {};
    std::function<void(void)> connectionLostCallback = [](void) {};
};

#endif // WIDGETCONNECT_H
