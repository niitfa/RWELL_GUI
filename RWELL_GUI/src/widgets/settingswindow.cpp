#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "settings.h"
#include <QMessageBox>

SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    setWindowTitle("Настройки");
    setFixedSize(300, 250);

    this->networkWidget = new NetworkSettings(this);

    /* add network widget */
    if(this->networkWidget)
    {
        this->addCustomWidget(this->networkWidget);
    }
}

SettingsWindow::~SettingsWindow()
{
    delete networkWidget;
    delete ui;
}

void SettingsWindow::show()
{
    this->QDialog::show();
    if(this->networkWidget)
    {
        this->networkWidget->show();
    }

    this->printStatus(QString());
}

void SettingsWindow::addCustomWidget(QWidget* widget)
{
    if(widget)
    {
        QListWidgetItem* newItem = new QListWidgetItem();
        newItem->setFlags(newItem->flags() & ~Qt::ItemIsEnabled);
        newItem->setSizeHint(widget->sizeHint());
        ui->listWidget_settings->addItem(newItem);
        ui->listWidget_settings->setItemWidget(newItem, widget);
    }
}

void SettingsWindow::printStatus(const QString& status)
{
    ui->label_statusbar->setText(status);
}

void SettingsWindow::on_pushButton_exit_clicked()
{
    this->close();
}

void SettingsWindow::on_pushButton_apply_clicked()
{
    if(this->networkWidget)
    {
        this->networkWidget->applySettings();
    }

    this->printStatus(QString("Настройки сохранены."));
}
