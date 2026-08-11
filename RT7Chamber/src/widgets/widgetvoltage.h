#ifndef WIDGETVOLTAGE_H
#define WIDGETVOLTAGE_H

#include <QWidget>
#include <QFont>
#include "rwell_client.h"

namespace Ui {
class WidgetVoltage;
}

class WidgetVoltage : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetVoltage(QWidget *parent = nullptr);
    ~WidgetVoltage();
    void setClient(RWELLClient*);
    int getInputVoltage();
private slots:
    void on_pushButton_changeVoltage_clicked();

private:
    Ui::WidgetVoltage *ui;
    QFont buttonsFont;
    RWELLClient* client = nullptr;
};

#endif // WIDGETVOLTAGE_H
