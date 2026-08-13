#include "statusled.h"
#include <QPalette>

StatusLED::StatusLED(QRadioButton* radioButton, QLabel* label)
    : radioButton { radioButton }, label { label }
{
    this->contentNoData = { QColor(220, 220, 220), "Нет данных" };
    this->contentReady = { QColor(150, 255, 150), "Готов" };
    this->contentProcess = { QColor(0xff, 0xff, 0x66), "В процессе" };
    this->contentError = { QColor(255, 100, 100), "Ошибка" };

    if(this->radioButton)
    {
        this->radioButton->setCheckable(false);
        this->radioButton->setEnabled(false);
        this->radioButton->setText("");
    }

    if(this->label)
    {
        QFont f;
        f.setFamily("Inter");
        f.setPixelSize(15);
        f.setWeight(50);
        this->label->setFont(f);
    }

    this->drawNoData();
}


void StatusLED::drawNoData()
{
    drawContent(this->contentNoData);
}

void StatusLED::drawProcess()
{
    drawContent(this->contentProcess);
}

void StatusLED::drawReady()
{
    drawContent(this->contentReady);
}

void StatusLED::drawError()
{
    drawContent(this->contentError);
}

void StatusLED::drawContent(const StatusLED::Content& content)
{
    if(this->radioButton)
    {
        QPalette palette;
        palette.setColor(QPalette::Base, content.color);
        palette.setColor(QPalette::Background, content.color);
        this->radioButton->setPalette(palette);
        this->radioButton->show();
    }
    if(this->label)
    {
        this->label->setText(content.text);
    }
}
