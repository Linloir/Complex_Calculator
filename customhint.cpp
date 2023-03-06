#include "customhint.h"
#include <QDebug>

CustomHint::CustomHint(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->setAlignment(Qt::AlignHCenter);
    label = new QLabel(this);
    label->setText("");
    label->setFont(QFont("Inconsolata", 12));
    vLayout->addWidget(label);
    //opacity = new QGraphicsOpacityEffect(this);
    //opacity->setOpacity(0);
    //this->setGraphicsEffect(opacity);
}

CustomHint::CustomHint(const QString & hintText, QColor color, QWidget *parent):
    QWidget(parent)
{
    //qDebug()<<"built";
    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->setAlignment(Qt::AlignHCenter);
    label = new QLabel(this);
    label->setText(hintText);
    label->setFont(QFont("Inconsolata", 12));
    label->setAlignment(Qt::AlignCenter);
    vLayout->addWidget(label);
    bgColor = color;
}

void CustomHint::paintEvent(QPaintEvent *event){
    //qDebug()<<"update";
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColor);
    painter.drawRoundedRect(this->rect(), 10, 10);
    //qDebug()<<"this"<<this->rect().width()<<","<<this->rect().height();
}
