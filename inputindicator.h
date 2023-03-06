#ifndef INPUTINDICATOR_H
#define INPUTINDICATOR_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>

class InputIndicator : public QWidget
{
    Q_OBJECT
public:
    InputIndicator(QColor color, QWidget* parent = nullptr);
    explicit InputIndicator(QWidget *parent = nullptr);
    void setValid(bool isValid){Bgcolor = isValid ? QColor(168,216,185) : QColor(241,124,103);}

private:
    QColor Bgcolor = QColor(168, 216, 185);
    void paintEvent(QPaintEvent*);

signals:

};

#endif // INPUTINDICATOR_H
