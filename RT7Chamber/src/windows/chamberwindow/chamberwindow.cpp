#include "chamberwindow.h"
#include "ui_chamberwindow.h"

ChamberWindow::ChamberWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChamberWindow)
{
    ui->setupUi(this);
    setWindowTitle("RT7 Chamber");
}

ChamberWindow::~ChamberWindow()
{
    delete ui;
}
