#ifndef CUSTOMHINT_H
#define CUSTOMHINT_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QLabel>
#include <QVBoxLayout>

class CustomHint : public QWidget
{
    Q_OBJECT
public:
    explicit CustomHint(QWidget *parent = nullptr);
    CustomHint(const QString & hintText, QColor color, QWidget *parent = nullptr);

private:
    QColor bgColor = QColor(168,216,185);
    QLabel* label;
    void paintEvent(QPaintEvent* event);

signals:

};

#endif // CUSTOMHINT_H
