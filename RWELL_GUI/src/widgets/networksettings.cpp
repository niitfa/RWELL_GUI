#include "networksettings.h"
#include "ui_networksettings.h"
#include <QFont>
#include <QSize>
#include <QHostAddress>
#include <iostream>
#include "settings.h"

NetworkSettings::NetworkSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetworkSettings)
{
    ui->setupUi(this);
    this->underlineTitle();
    this->showAll();
}

NetworkSettings::~NetworkSettings()
{
    delete ui;
}

void NetworkSettings::applySettings()
{
    // change ip input field
    Settings::set(Settings::Key::IP, Settings::Section::Main) =
            ui->lineEdit_IP->text();
    Settings::set(Settings::Key::Port, Settings::Section::Main) =
            ui->lineEdit_port->text();
}

void NetworkSettings::show()
{
    QWidget::show();
    this->showAll();
}

void NetworkSettings::underlineTitle()
{
    ui->label_title->font();
    QFont font = ui->label_title->font();
    font.setUnderline(true);
    font.setWeight(QFont::Weight::Normal);
    ui->label_title->setFont(font);
}

void NetworkSettings::showAll()
{
    ui->lineEdit_IP->setText(
                Settings::get(Settings::Key::IP, Settings::Section::Main).toString()
                );
    ui->lineEdit_port->setText(
                Settings::get(Settings::Key::Port, Settings::Section::Main).toString()
                );
}

QString NetworkSettings::validatePort(const QString& strPort)
{
    const int uint16_max = 65535;
    try {
        int resInt = strPort.toInt();
        if(resInt < 0 || resInt > uint16_max)
        {
            return QString::number(
                        Settings::get(Settings::Key::Port, Settings::Section::Main).toInt()
                        );
        }
        else
        {
            return QString::number(resInt);
        }
    } catch (...)
    {
        return QString::number(
                    Settings::get(Settings::Key::Port, Settings::Section::Main).toInt()
                    );
    }
}

void NetworkSettings::on_lineEdit_IP_editingFinished()
{
    QHostAddress ipv4addr ( ui->lineEdit_IP->text() );
    // invalid address check
    if(QAbstractSocket::IPv4Protocol != ipv4addr.protocol())
    {
        ui->lineEdit_IP->setText(
                     Settings::get(Settings::Key::IP, Settings::Section::Main).toString()
                    );
    }
}

void NetworkSettings::on_lineEdit_port_editingFinished()
{
    QString validated = this->validatePort(
                ui->lineEdit_port->text()
                );
    ui->lineEdit_port->setText(validated);
}
