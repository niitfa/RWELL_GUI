#include "noisemeasurementwindow.h"
#include "ui_noisemeasurementwindow.h"

NoiseMeasurementWindow::NoiseMeasurementWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NoiseMeasurementWindow)
{
    ui->setupUi(this);
}

NoiseMeasurementWindow::~NoiseMeasurementWindow()
{
    delete ui;
}
