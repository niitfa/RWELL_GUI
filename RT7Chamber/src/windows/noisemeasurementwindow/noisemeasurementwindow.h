#ifndef NOISEMEASUREMENTWINDOW_H
#define NOISEMEASUREMENTWINDOW_H

#include <QDialog>

namespace Ui {
class NoiseMeasurementWindow;
}

class NoiseMeasurementWindow : public QDialog
{
    Q_OBJECT

public:
    explicit NoiseMeasurementWindow(QWidget *parent = nullptr);
    ~NoiseMeasurementWindow();

private:
    Ui::NoiseMeasurementWindow *ui;
};

#endif // NOISEMEASUREMENTWINDOW_H
