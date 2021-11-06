#include "func.h"
#include <QDebug>

func::func()
{

}

func::func(const func* f){
    name = f->name;
    partialVar = f->partialVar;
    size = f->size;
    expression = new expr(f->expression);
}

func::func(const QString& _name, const QVector<QString>& _var, expr* _exp):
    name(_name),
    expression(_exp)
{
    size = _var.size();
    partialVar.clear();
    for(int i = 0; i < size; i++){
        partialVar.insert(_var[i], {0,0});
    }
}

void func::Assign(QString var, expr* exp){
    var.remove(' ');
    expression->Assign(var, exp);
}

void func::Assign(QVector<expr*> exprList){
    int n = size < exprList.size() ? size : exprList.size();
    for(int i = 0; i < n; i++){
        QString name = QString::fromUtf8("__temp__").append(QString::asprintf("%d", i));
        expr* temp = new expr("__variable__", name);
        qDebug() << partialVar.keys()[i] << "->" << name;
        expression->Assign(partialVar.keys()[i], temp);
        delete temp;
    }
    for(int i = 0; i < n; i++){
        QString name = QString::fromUtf8("__temp__").append(QString::asprintf("%d", i));
        qDebug() << name << "->" << exprList[i]->variable;
        expression->Assign(name, exprList[i]);
    }
}

expr::expr()
{
    function = nullptr;
}

expr::expr(const expr* e){
    constraintFlag = e->constraintFlag;
    op = e->op;
    for(int i = 0; i < e->exprList.size(); i++){
        expr* newExpr = new expr(e->exprList[i]);
        exprList.push_back(newExpr);
    }
    constant = e->constant;
    variable = e->variable;
    function = e->function;
}

expr::expr(const QString & _op, const double & _c):op(_op),constant(_c){
    function = nullptr;
}

expr::expr(const QString & _op, const QString & _var):op(_op),variable(_var){
    function = nullptr;
}
expr::expr(const QString & _op, const QVector<expr*> & _exprList, func* _function):
    op(_op),exprList(_exprList),function(_function)
{

}
expr::expr(const QString & _op, const QVector<expr*> & _exprList):
    op(_op),exprList(_exprList)
{
    function = nullptr;
}
QString expr::Print(int flag){
    qDebug() << op;
    if(flag & Stat::INFIX){
        if(op == "__constant__"){
            QString res;
            if(constant < 0)
                res.append("(");
            res.append(QString::asprintf("%g", constant));
            if(constant < 0)
                res.append(")");
            return res;
        }
        else if(op == "__variable__"){
            return variable;
        }
        else{
            if(operatorPool.contains(op)){
                if(operatorPool.value(op)[0] != 2){
                    QString res;
                    res.append(op);
                    res.append("(");
                    res.append(exprList[0]->Print(Stat::INFIX));
                    for(int i = 1; i < exprList.size(); i++){
                        res.append(',');
                        res.append(exprList[i]->Print(Stat::INFIX));
                    }
                    res.append(")");
                    return res;
                }
                else{
                    QString res;
                    if(exprList[0]->function != nullptr || (operatorPool.contains(exprList[0]->Op()) && operatorPool.value(exprList[0]->Op())[1] < operatorPool.value(op)[1])){
                        res.append("(");
                        res.append(exprList[0]->Print(Stat::INFIX));
                        res.append(")");
                    }
                    else
                        res.append(exprList[0]->Print(Stat::INFIX));
                    res.append(op);
                    if(exprList[0]->function != nullptr || (operatorPool.contains(exprList[1]->Op()) && operatorPool.value(exprList[1]->Op())[1] < operatorPool.value(op)[1])){
                        res.append("(");
                        res.append(exprList[1]->Print(Stat::INFIX));
                        res.append(")");
                    }
                    else
                        res.append(exprList[1]->Print(Stat::INFIX));
                    return res;
                }
            }
            else{
                qDebug() << "function"<< function->Name();
                //function
                //use replace to expand the expression
                func* assigned = new func(function);
                //for(int i = 0; i < assigned->Size(); i++){
                //    QString name = QString::fromUtf8("__temp__").append(QString::asprintf("%d", i));
                //    expr* temp = new expr("__variable__", name);
                //    assigned->Assign(assigned->VarName(i), temp);
                //
                //    delete temp;
                //}
                //for(int i = 0; i < assigned->Size(); i++){
                //    QString name = QString::fromUtf8("__temp__").append(QString::asprintf("%d", i));
                //    assigned->Assign(name, exprList[i]);
                //}
                assigned->Assign(exprList);
                QString res;
                res.append(assigned->Expr()->Print(Stat::INFIX));
                //QString res;
                //res.append(function->Name());
                //res.append("(");
                //res.append(exprList[0]->Print(Stat::INFIX));
                //for(int i = 1; i < exprList.size(); i++){
                //    res.append(",");
                //    res.append(exprList[i]->Print(Stat::INFIX));
                //}
                //res.append(")");
                return res;
            }
        }
    }
    else{
        return "";
    }
}

void expr::Assign(QString var, expr *exp){
    if(op == "__constant__")    return;
    else if(op == "__variable__"){
        if(variable == var){
            op = exp->op;
            exprList = exp->exprList;
            constant = exp->constant;
            variable = exp->variable;
            function = exp->function;
        }
        return;
    }
    else if(function != nullptr){
        func* assignedFunc = new func(function);
        assignedFunc->Assign(var, exp);
        function = assignedFunc;
        return;
    }
    else{
        for(int i = 0; i < exprList.size(); i++)
            exprList[i]->Assign(var, exp);
    }
}
