#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QAbstractAnimation>
#include "scrollareacustom.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    ScrollAreaCustom* globalVar;
    ScrollAreaCustom* globalFunc;
    Ui::MainWindow *ui;
    void paintEvent(QPaintEvent*);
    void InitInterface();
};
#endif // MAINWINDOW_H
