#ifndef STATUSLED_H
#define STATUSLED_H

#include <QRadioButton>
#include <QLabel>

class StatusLED
{
    struct Content
    {
        QColor color;
        QString text;
    };

public:
    StatusLED(QRadioButton* radioButton, QLabel* label = nullptr);
    ~StatusLED() = default;

    void drawNoData();
    void drawReady();
    void drawProcess();
    void drawError();

private:
    QRadioButton* radioButton = nullptr;
    QLabel* label = nullptr;
    StatusLED::Content contentNoData, contentReady,
        contentProcess, contentError;

    void drawContent(const StatusLED::Content& content);
};

#endif // STATUSLED_H
