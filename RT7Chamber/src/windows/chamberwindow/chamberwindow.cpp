#include "chamberwindow.h"
#include "ui_chamberwindow.h"

ChamberWindow::ChamberWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChamberWindow)
{
    ui->setupUi(this);
}

ChamberWindow::~ChamberWindow()
{
    delete ui;
}
