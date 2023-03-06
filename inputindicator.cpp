#include "inputindicator.h"

InputIndicator::InputIndicator(QWidget *parent) : QWidget(parent)
{

}

InputIndicator::InputIndicator(QColor color, QWidget *parent) :
    QWidget(parent), Bgcolor(color)
{

}

void InputIndicator::paintEvent(QPaintEvent *){
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Bgcolor);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawRoundedRect(QRect(0, 0, this->width(), this->height()), 10, 10);
    painter.drawRect(QRect(5, 0, this->width() - 5, this->height()));
}
