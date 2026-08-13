#ifndef NETWORKSETTINGS_H
#define NETWORKSETTINGS_H

#include <QWidget>
#include <QString>
#include <QSize>

namespace Ui {
class NetworkSettings;
}

class NetworkSettings : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkSettings(QWidget *parent = nullptr);
    ~NetworkSettings();
    void applySettings();
    void show();

private slots:

    void on_lineEdit_IP_editingFinished();
    void on_lineEdit_port_editingFinished();

private:
    Ui::NetworkSettings *ui;

private:
    void underlineTitle();
    void showAll();
    QString validatePort(const QString&);
};

#endif // NETWORKSETTINGS_H
