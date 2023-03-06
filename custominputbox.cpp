#include "custominputbox.h"

CustomInputBox::CustomInputBox(QWidget *parent) : QWidget(parent)
{

    indicator = new InputIndicator(QColor(168, 216, 185), this);
    indicator->move(0, 0);

    inputLabel = new QLineEdit(this);
    inputLabel->setText("");
    inputLabel->setFrame(false);
    inputLabel->setFocusPolicy(Qt::StrongFocus);
    //inputBoxPos = QPoint(15, this->height() / 2 - 10);
    connect(inputLabel, &QLineEdit::textEdited, [=](){
        inputText = inputLabel->text();
        hideOutput();
        verify();
        update();
    });
    connect(inputLabel, &QLineEdit::returnPressed, [=](){
        build();
        update();
    });

    outputLabel = new QLabel(this);
    opac = new QGraphicsOpacityEffect(outputLabel);
    opac->setOpacity(0);
    outputLabel->setGraphicsEffect(opac);

    //QTimer* timer = new QTimer(this);
    //connect(timer, &QTimer::timeout, [=](){
    //    inputLabel->move(QPoint(15, this->height() / 2 - 10));
    //});
    //timer->start(10);
}

void CustomInputBox::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255));
    painter.drawRoundedRect(this->rect(), 10, 10);
    indicator->resize(8, this->height());

    fontSize = 18;
    QPainter textPainter(this);
    QFont textFont(font, fontSize);
    textPainter.setFont(textFont);
    int widthOfText = textPainter.fontMetrics().size(Qt::TextSingleLine, inputLabel->text()).width();
    while (widthOfText > this->width() - 30) {
        fontSize--;
        textFont = QFont(font, fontSize);
        textPainter.setFont(textFont);
        widthOfText = textPainter.fontMetrics().size(Qt::TextSingleLine, inputLabel->text()).width();
    }
    int heightOfText = textPainter.fontMetrics().ascent() - textPainter.fontMetrics().descent() + textPainter.fontMetrics().leading();
    //test.drawRect(this->width() / 2 - widthOfText / 2, this->height() / 2 - heightOfText / 2, widthOfText, heightOfText);
    //textPainter.drawText(15, this->height() / 2 + heightOfText / 2, inputText);
    //inputLabel->setText(inputText);
    inputLabel->setFont(textFont);
    inputLabel->resize(this->width() - 30, heightOfText + 15);
    inputLabel->move(inputBoxPos);

    //fontSize = 18;
    //textFont = QFont(font, fontSize);
    //textPainter.setFont(textFont);
    //widthOfText = textPainter.fontMetrics().size(Qt::TextSingleLine, outputLabel->text()).width();
    //while (widthOfText > this->width() - 30) {
    //    fontSize--;
    //    textFont = QFont(font, fontSize);
    //    textPainter.setFont(textFont);
    //    widthOfText = textPainter.fontMetrics().size(Qt::TextSingleLine, outputLabel->text()).width();
    //}
    outputLabel->setFont(textFont);
    outputLabel->resize(this->width() - 30, heightOfText + 15);
    outputLabel->move(QPoint(15, this->height() / 2));

    textFont = QFont(font, 12);
    textPainter.setFont(textFont);
    textPainter.setRenderHint(QPainter::TextAntialiasing);
    textPainter.setPen(isValid ? QColor(168,216,185) : QColor(241,124,103));
    textPainter.drawText(15, 20, state);


    //qDebug()<<"w,h"<<this->width()<<","<<this->height();
}

void CustomInputBox::resizeEvent(QResizeEvent *event){
    if(hasOutput)
        inputBoxPos = QPoint(15, this->height() / 2 - 30);
    else
        inputBoxPos = QPoint(15, this->height() / 2 - 10);
    update();
    emit sizechanged();
}

void CustomInputBox::verify(){
    try {
        int stat = QuickVerify(inputText);
        QString statStr;
        if(stat == Stat::UNDEFINED)
            statStr.append("Unknown");
        if(stat & Stat::EQUATION)
            statStr.append("Equation");
        else if(stat & Stat::EXPR)
            statStr.append("Direct expression");
        if(stat & Stat::INFIX)
            statStr.append(" | INFIX MODE");
        else if(stat & Stat::PREFIX)
            statStr.append(" | PREFIX MODE");
        state = statStr;
        isValid = true;
        indicator->setValid(true);
        showHint("Valid input. Press ENTER to build", QColor(168, 216, 185));
        //Build(inputText, stat);
        //emit poolchanged();
    }  catch (syntaxError& se) {
        isValid = false;
        indicator->setValid(false);
        showError(se);
        update();
    }
}

void CustomInputBox::build(){
    try {
        int stat = QuickVerify(inputText);
        QString statStr;
        if(stat == Stat::UNDEFINED)
            statStr.append("Unknown");
        if(stat & Stat::EQUATION)
            statStr.append("Equation");
        else if(stat & Stat::EXPR)
            statStr.append("Direct expression");
        if(stat & Stat::INFIX)
            statStr.append(" | INFIX MODE");
        else if(stat & Stat::PREFIX)
            statStr.append(" | PREFIX MODE");
        state = statStr;
        isValid = true;
        indicator->setValid(true);
        func* res = Build(inputText, stat);
        QString output;
        output.append(res->Expr()->Print(Stat::INFIX));
        output.append(" | merge: ");
        res->Merge();
        output.append(res->Expr()->Print(Stat::INFIX));
        outputLabel->setText(output);
        showOutput();
        showHint("Expression built!", QColor(168, 216, 185));
        emit poolchanged();
    }  catch (syntaxError& se) {
        isValid = false;
        indicator->setValid(false);
        showError(se);
        update();
    }
}

void CustomInputBox::showError(syntaxError &error){
    QString info;
    info.append("Syntax error ");
    info.append(QString::asprintf("0x%x", error.code()));
    info.append(" at ");
    info.append(QString::asprintf("%d : ", error.pos()));
    info.append(" ");
    info.append(error.info());
    QString space;
    space.append("<span style=\"color:#E56600;\">");
    for(int i = 0; i < error.pos(); i++)
        space.append("&nbsp;");
    space.append("^");
    for(int i = 0; i < inputText.length() - error.pos(); i++)
        space.append("~");
    space.append("</span>");
    outputLabel->setTextFormat(Qt::RichText);
    outputLabel->setText(space);
    showOutput();
    showHint(info, QColor(241,124,103));
}

void CustomInputBox::showHint(const QString & string, QColor color){
    CustomHint* newHint = new CustomHint(string, color, this);
    connect(this, &CustomInputBox::sizechanged, [=](){
        newHint->resize(this->width() - 30, 50);
        newHint->move(15, this->height() - 60);});
    newHint->show();
    //newHint->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    newHint->resize(this->width() - 30, 50);
    newHint->move(15, this->height() - 10);

    QGraphicsOpacityEffect* opacity = new QGraphicsOpacityEffect(newHint);
    opacity->setOpacity(0);
    newHint->setGraphicsEffect(opacity);
    QPropertyAnimation* fadeIn = new QPropertyAnimation(opacity, "opacity");
    fadeIn->setStartValue(0);
    fadeIn->setEndValue(1);
    fadeIn->setDuration(300);
    QPropertyAnimation* move = new QPropertyAnimation(newHint, "pos");
    move->setStartValue(newHint->pos());
    move->setEndValue(QPoint(15, this->height() - 60));
    move->setDuration(500);
    move->setEasingCurve(QEasingCurve::OutQuart);
    QParallelAnimationGroup* group = new QParallelAnimationGroup(newHint);
    group->addAnimation(fadeIn);
    group->addAnimation(move);
    group->start();

    QTimer* countDown = new QTimer(newHint);
    countDown->setSingleShot(true);
    connect(countDown, &QTimer::timeout, [=](){hideHint(newHint);});

    countDown->start(3000);
}

void CustomInputBox::hideHint(CustomHint *hint){
    QGraphicsOpacityEffect* opacity = new QGraphicsOpacityEffect(hint);
    opacity->setOpacity(1);
    hint->setGraphicsEffect(opacity);
    QPropertyAnimation* fadeOut = new QPropertyAnimation(opacity, "opacity");
    fadeOut->setStartValue(1);
    fadeOut->setEndValue(0);
    fadeOut->setDuration(300);
    fadeOut->start();
}

void CustomInputBox::showOutput(){
    hasOutput = true;
    QParallelAnimationGroup* group = new QParallelAnimationGroup(this);
    //QPropertyAnimation* move = new QPropertyAnimation(inputLabel, "pos");
    //move->setStartValue(inputLabel->pos());
    //move->setEndValue(QPoint(15, this->height() / 2 - 120));
    //move->setEasingCurve(QEasingCurve::InOutQuad);
    //move->setDuration(300);
    //qDebug()<<"m";
    inputBoxPos = QPoint(15, this->height() / 2 - 30);
    //QPropertyAnimation* fadeIn = new QPropertyAnimation(opac, "opacity");
    //fadeIn->setStartValue(opac->opacity());
    //fadeIn->setEndValue(1);
    //fadeIn->setDuration(400);
    //group->addAnimation(move);
    //group->addAnimation(fadeIn);
    //group->start();
    opac->setOpacity(1);
}

void CustomInputBox::hideOutput(){
    if(hasOutput){
        hasOutput = false;
        QParallelAnimationGroup* group = new QParallelAnimationGroup();
        //QPropertyAnimation* move = new QPropertyAnimation(inputLabel, "pos");
        //move->setStartValue(inputLabel->pos());
        //move->setEndValue(QPoint(15, this->height() / 2 - 60));
        //move->setEasingCurve(QEasingCurve::InOutQuad);
        //move->setDuration(300);
        //qDebug()<<"b";
        inputBoxPos = QPoint(15, this->height() / 2 - 10);
        //QPropertyAnimation* fadeIn = new QPropertyAnimation(opac, "opacity");
        //fadeIn->setStartValue(opac->opacity());
        //fadeIn->setEndValue(0);
        //fadeIn->setDuration(400);
        //group->addAnimation(move);
        //group->addAnimation(fadeIn);
        //group->start();
        opac->setOpacity(0);
    }
}
