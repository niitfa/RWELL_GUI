#ifndef CHAMBERWINDOW_H
#define CHAMBERWINDOW_H

#include <QDialog>

namespace Ui {
class ChamberWindow;
}

class ChamberWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ChamberWindow(QWidget *parent = nullptr);
    ~ChamberWindow();

private:
    Ui::ChamberWindow *ui;
};

#endif // CHAMBERWINDOW_H
