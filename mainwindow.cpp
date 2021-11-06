#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "corefunction.h"

#include <QDebug>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    InitializeOperatorPool();
    InitializeVariablePool();
    InitializeFunctionPool();
    ui->InputBox->setFont(QFont("Inconsolata", 15));
    ui->OutputBox->setFont(QFont("Inconsolata", 15));
    connect(ui->InputBox, &QTextBrowser::textChanged, [=](){
        try {
            QuickVerify(ui->InputBox->toPlainText());
            ui->OutputBox->setText(ui->InputBox->toPlainText());
        }  catch (syntaxError& myError) {
            QString space;
            space.append(ui->InputBox->toPlainText());
            space.append("\n");
            for(int i = 0; i < myError.pos(); i++)
                space.append(" ");
            space.append("^");
            for(int i = 0; i < ui->InputBox->toPlainText().length() - myError.pos(); i++)
                space.append("~");
            space.append("\n");
            space.append(myError.info());
            ui->OutputBox->setText(space);
        }
    });
    connect(ui->pushButton, &QPushButton::clicked, [=](){
        try {
            int stat = QuickVerify(ui->InputBox->toPlainText());
            ui->OutputBox->setText(ui->InputBox->toPlainText());
            func* func = Build(ui->InputBox->toPlainText(), stat);
            ui->OutputBox->setText(func->Expr()->Print(Stat::INFIX));
        }  catch (syntaxError& myError) {
            QString space;
            space.append(ui->InputBox->toPlainText());
            space.append("\n");
            for(int i = 0; i < myError.pos(); i++)
                space.append(" ");
            space.append("^");
            for(int i = 0; i < ui->InputBox->toPlainText().length() - myError.pos(); i++)
                space.append("~");
            space.append("\n");
            space.append(myError.info());
            ui->OutputBox->setText(space);
        }
    });
   //QString a("f(x) = + x 3");
   //qDebug() << a;
   //try {
   //    QuickVerify(a);
   //}  catch (syntaxError& myError) {
   //    QString space;
   //    for(int i = 0; i < myError.pos(); i++)
   //        space.append(" ");
   //    space.append("^");
   //    for(int i = 0; i < a.length() - myError.pos(); i++)
   //        space.append("~");
   //    qDebug() << a;
   //    qDebug() << space;
   //    qDebug() << myError.info() << "at" << myError.pos();
   //}
    qDebug() << "test";
}

MainWindow::~MainWindow()
{
    delete ui;
}

