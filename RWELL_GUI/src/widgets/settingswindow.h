#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include "networksettings.h"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();
    void show();

private slots:
    void on_pushButton_exit_clicked();

    void on_pushButton_apply_clicked();

private:
    Ui::SettingsWindow *ui;
    NetworkSettings* networkWidget = nullptr;

private:
    void addCustomWidget(QWidget*);
    void printStatus(const QString&);
};

#endif // SETTINGSWINDOW_H
