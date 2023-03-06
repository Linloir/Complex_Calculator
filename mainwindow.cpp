#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "corefunction.h"
#include "custominputbox.h"

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

    ui->globalVar->clear();
    for(int i = 0; i < variablePool.size(); i++){
        QLabel* var = new QLabel;
        QString str;
        str.append(variablePool.keys().at(i));
        str.append(" = ");
        str.append(variablePool.values().at(i)[0] == 0?"UNDEFINED":QString::asprintf("%g", variablePool.values().at(i)[1]));
        var->setText(str);
        var->setFont(QFont("Inconsolata", 15));
        var->resize(ui->globalVar->width(), 30);
        ui->globalVar->addWidget(var);
    }

    connect(ui->InputBoxWidget, &CustomInputBox::poolchanged, [=](){
        qDebug() << "changed";
        ui->globalVar->clear();
        for(int i = 0; i < variablePool.size(); i++){
            QLabel* var = new QLabel;
            QString str;
            str.append(variablePool.keys().at(i));
            str.append(" = ");
            str.append(variablePool.values().at(i)[0] == 0?"UNDEFINED":QString::asprintf("%g", variablePool.values().at(i)[1]));
            var->setText(str);
            var->setFont(QFont("Inconsolata", 15));
            var->resize(ui->globalVar->width(), 30);
            ui->globalVar->addWidget(var);
        }
        ui->globalFunc->clear();
        for(int i = 0; i < functionPool.size(); i++){
            QLabel* funcl = new QLabel;
            QString str;
            str.append(functionPool.values().at(i)->Print(Stat::INFIX));
            funcl->setText(str);
            funcl->setFont(QFont("Inconsolata", 15));
            funcl->resize(ui->globalVar->width(), 30);
            ui->globalFunc->addWidget(funcl);
        }
    });

    //ui->InputBox->setFont(QFont("Inconsolata", 15));
    //ui->OutputBox->setFont(QFont("Inconsolata", 15));
    //
    //QString globalVar;
    //for(int i = 0; i < variablePool.size(); i++){
    //    globalVar.append(variablePool.keys().at(i));
    //    globalVar.append("=");
    //    globalVar.append(variablePool.values().at(i)[0] == 1 ? QString::asprintf("%g",variablePool.values().at(i)[1]) : "UNDEFINED");
    //    globalVar.append("\n");
    //}
    //ui->GlobalVar->setText(globalVar);
    //
    //connect(ui->InputBox, &QTextBrowser::textChanged, [=](){
    //    try {
    //        QuickVerify(ui->InputBox->toPlainText());
    //        ui->OutputBox->setText(ui->InputBox->toPlainText());
    //    }  catch (syntaxError& myError) {
    //        QString space;
    //        space.append(ui->InputBox->toPlainText());
    //        space.append("\n");
    //        for(int i = 0; i < myError.pos(); i++)
    //            space.append(" ");
    //        space.append("^");
    //        for(int i = 0; i < ui->InputBox->toPlainText().length() - myError.pos(); i++)
    //            space.append("~");
    //        space.append("\n");
    //        space.append(myError.info());
    //        ui->OutputBox->setText(space);
    //    }
    //});
    //connect(ui->pushButton, &QPushButton::clicked, [=](){
    //    try {
    //        int stat = QuickVerify(ui->InputBox->toPlainText());
    //        ui->OutputBox->setText(ui->InputBox->toPlainText());
    //        func* func = Build(ui->InputBox->toPlainText(), stat);
    //        ui->OutputBox->setText(func->Expr()->Print(Stat::INFIX));
    //        QString funct;
    //        for(int i = 0; i < functionPool.size(); i++){
    //            funct.append(functionPool.values().at(i)->Print(Stat::INFIX));
    //            funct.append("\n");
    //        }
    //        ui->func->setText(funct);
    //
    //        QString globalVar;
    //        for(int i = 0; i < variablePool.size(); i++){
    //            globalVar.append(variablePool.keys().at(i));
    //            globalVar.append("=");
    //            globalVar.append(variablePool.values().at(i)[0] == 1 ? QString::asprintf("%g",variablePool.values().at(i)[1]) : "UNDEFINED");
    //            globalVar.append("\n");
    //        }
    //        ui->GlobalVar->setText(globalVar);
    //    }  catch (syntaxError& myError) {
    //        QString space;
    //        space.append(ui->InputBox->toPlainText());
    //        space.append("\n");
    //        for(int i = 0; i < myError.pos(); i++)
    //            space.append(" ");
    //        space.append("^");
    //        for(int i = 0; i < ui->InputBox->toPlainText().length() - myError.pos(); i++)
    //            space.append("~");
    //        space.append("\n");
    //        space.append(myError.info());
    //        ui->OutputBox->setText(space);
    //    }
    //});
    //connect(ui->Merge, &QPushButton::clicked, [=](){
    //    try {
    //        int stat = QuickVerify(ui->InputBox->toPlainText());
    //        ui->OutputBox->setText(ui->InputBox->toPlainText());
    //        func* func = Build(ui->InputBox->toPlainText(), stat);
    //        func->Merge();
    //        ui->OutputBox->setText(func->Expr()->Print(Stat::INFIX));
    //
    //
    //        QString funct;
    //        for(int i = 0; i < functionPool.size(); i++){
    //            funct.append(functionPool.values().at(i)->Print(Stat::INFIX));
    //            funct.append("\n");
    //        }
    //        ui->func->setText(funct);
    //
    //        QString globalVar;
    //        for(int i = 0; i < variablePool.size(); i++){
    //            globalVar.append(variablePool.keys().at(i));
    //            globalVar.append("=");
    //            globalVar.append(variablePool.values().at(i)[0] == 1 ? QString::asprintf("%g",variablePool.values().at(i)[1]) : "UNDEFINED");
    //            globalVar.append("\n");
    //        }
    //        ui->GlobalVar->setText(globalVar);
    //    }  catch (syntaxError& myError) {
    //        QString space;
    //        space.append(ui->InputBox->toPlainText());
    //        space.append("\n");
    //        for(int i = 0; i < myError.pos(); i++)
    //            space.append(" ");
    //        space.append("^");
    //        for(int i = 0; i < ui->InputBox->toPlainText().length() - myError.pos(); i++)
    //            space.append("~");
    //        space.append("\n");
    //        space.append(myError.info());
    //        ui->OutputBox->setText(space);
    //    }
    //});
    //connect(ui->Diff, &QPushButton::clicked, [=](){
    //    Build("DIFF(X,Y,Z)=(X-Y)/Z", Stat::EQUATION|Stat::INFIX);
    //    QString funct;
    //    for(int i = 0; i < functionPool.size(); i++){
    //        funct.append(functionPool.values().at(i)->Print(Stat::INFIX));
    //        funct.append("\n");
    //    }
    //    ui->func->setText(funct);
    //});
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
    //qDebug() << "test";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event){
    QPainter painter(this);
}
