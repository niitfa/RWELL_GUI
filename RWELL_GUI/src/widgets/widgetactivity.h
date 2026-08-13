#ifndef WIDGETACTIVITY_H
#define WIDGETACTIVITY_H

#include <QWidget>
#include <QFont>
#include "rwell_client.h"

namespace Ui {
class WidgetActivity;
}

class WidgetActivity : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetActivity(QWidget *parent = nullptr);
    ~WidgetActivity();
    void setClient(RWELLClient*);

    void setMeasuresCompleted(int);

private slots:
    void on_pushButton_startMeasurement_clicked();
    void on_pushButton_resetMeasurement_clicked();

private:
    Ui::WidgetActivity *ui;
    void drawStartStateButton();
    void drawStopStateButton();
    void drawCurrentStartButtonState();
private:
    bool buttonInStartState = 1;
    QFont buttonsFont;
    RWELLClient* client = nullptr;
};

#endif // WIDGETACTIVITY_H
