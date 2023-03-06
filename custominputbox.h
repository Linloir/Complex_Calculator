#ifndef CUSTOMINPUTBOX_H
#define CUSTOMINPUTBOX_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QString>
#include <QTextBrowser>
#include <QLabel>
#include <QLayout>
#include <QKeyEvent>
#include <QTimer>
#include <QLineEdit>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <Windows.h>
#include <WinUser.h>
#include "inputindicator.h"
#include "corefunction.h"
#include "customhint.h"

class CustomInputBox : public QWidget
{
    Q_OBJECT
public:
    explicit CustomInputBox(QWidget *parent = nullptr);
    QString text(){return inputText;}

private:
    int fontSize = 18;
    QString font = "Inconsolata";
    QString inputText = "v = ";
    QString state = "Unknown";
    QLineEdit* inputLabel;
    QLabel* outputLabel;
    QGraphicsOpacityEffect* opac;
    InputIndicator* indicator;
    QPoint inputBoxPos;
    bool hasOutput = false;
    bool isValid;
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);
    //void keyPressEvent(QKeyEvent*);
    void verify();
    void build();
    void showHint(const QString &, QColor);
    void hideHint(CustomHint* hint);
    void showError(syntaxError& error);
    void showOutput();
    void hideOutput();

signals:
    //void textchanged(QString);
    void statchanged(QString);
    void poolchanged();
    void sizechanged();
};

#endif // CUSTOMINPUTBOX_H
