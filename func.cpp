#include "func.h"
#include "myerrors.h"
#include <QDebug>

func::func()
{

}

func::func(const func* f){
    name = f->name;
    parameters = f->parameters;
    partialVar = f->partialVar;
    size = f->size;
    expression = new expr(f->expression);
}

func::func(const QString & _name, expr* _exp):name(_name),expression(_exp){

}

func::func(const QString & _name, const QVector<QString> & _par, const QMap<QString, QList<double>> & _var, expr* _exp):
    name(_name),
    parameters(_par),
    partialVar(_var),
    expression(_exp)
{
    //qDebug() << "parsize:" << parameters.size() << "varsize:" << partialVar.size();
    size = _var.size();
}

QString func::Print(int flag){
    QString res;
    res.append(name);
    res.append("(");
    res.append(parameters[0]);
    for(int i = 1; i < parameters.size(); i++){
        res.append(",");
        res.append(parameters[i]);
    }
    res.append(")=");
    res.append(expression->Print(flag));
    return res;
}

void func::Assign(QString var, expr* exp){
    var.remove(' ');
    expression->Assign(var, exp);
    if(parameters.contains(var) && exp->Op() == "__constant__"){
        partialVar.find(var).value()[0] = 1;
        partialVar.find(var).value()[1] = exp->Constant();
    }
}

void func::Assign(QVector<expr*> exprList){
    int n = size < exprList.size() ? size : exprList.size();
    for(int i = 0; i < n; i++){
        QString name = QString::fromUtf8("__temp__").append(this->name).append(QString::asprintf("__%d", i));
        expr* temp = new expr("__variable__", name);
        expression->Assign(parameters[i], temp);
        delete temp;
    }
    for(int i = 0; i < n; i++){
        QString name = QString::fromUtf8("__temp__").append(this->name).append(QString::asprintf("__%d", i));
        expression->Assign(name, exprList[i]);
        if(exprList[i]->Op() == "__constant__"){
            partialVar.find(parameters[i]).value()[0] = 1;
            partialVar.find(parameters[i]).value()[1] = exprList[i]->Constant();
        }
        else if(exprList[i]->Op() == "__variable__"){
            partialVar.remove(parameters[i]);
            parameters[i] = exprList[i]->Variable();
            partialVar.insert(parameters[i], {0,0});
        }
    }
}

void func::Merge(){
    expression->Merge(partialVar);
}

void func::Diff(){
    expr* c = new expr("__constant__", 3.14159265358);
    Assign("y", c);
    QVector<QString> test;
    test.push_back("x");
    test.push_back("y");
    expression->Diff(test, "x");
    expression->Assign(3.14159265358, "y");
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
    op(_op),function(_function),exprList(_exprList)
{

}
expr::expr(const QString & _op, const QVector<expr*> & _exprList):
    op(_op),exprList(_exprList)
{
    function = nullptr;
}
QString expr::Print(int flag){
    //qDebug() << op;
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
                    if(exprList[1]->function != nullptr || (operatorPool.contains(exprList[1]->Op()) && operatorPool.value(exprList[1]->Op())[1] <= operatorPool.value(op)[1])){
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
                //qDebug() << "function"<< function->Name();
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
                //qDebug()<<"assigning";
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
        //localize
        assignedFunc->Assign(exprList);
        assignedFunc->Assign(var, exp);
        function = assignedFunc;
        return;
    }
    else{
        for(int i = 0; i < exprList.size(); i++)
            exprList[i]->Assign(var, exp);
    }
}

void expr::Assign(double key, QString var){
    if(op == "__constant__"){
        if(constant == key){
            op = "__variable__";
            constant = 0;
            variable = var;
        }
        return;
    }
    else if(op == "__variable__"){
        return;
    }
    else if(function != nullptr){
        return;
    }
    else{
        for(int i = 0; i < exprList.size(); i++)
            exprList[i]->Assign(key, var);
    }
}

void expr::Merge(const QMap<QString, QVector<double>>& partialVar){
    for(int i = 0; i < exprList.size(); i++){
        exprList[i]->Merge(partialVar);
    }
    if(op == "__constant__"){
        return;
    }
    else if(op == "__variable__"){
        if(partialVar.contains(variable) && partialVar.value(variable)[0] == 1){
            op = "__constant__";
            constant = partialVar.value(variable).at(1);
            variable = "";
        }
        else if(variablePool.contains(variable) && variablePool.value(variable)[0] == 1){
            op = "__constant__";
            constant = variablePool.value(variable).at(1);
            variable = "";
        }
    }
    else if(op == "__function__"){
        func* merged = new func(function);
        merged->Assign(exprList);
        merged->Merge();
        expr* mergedExpr = merged->Expr();
        constraintFlag = mergedExpr->constraintFlag;
        op = mergedExpr->op;
        constant = mergedExpr->constant;
        variable = mergedExpr->variable;
        function = mergedExpr->function;
        exprList = mergedExpr->exprList;
        delete  merged;
    }
    else{
        for(int i = 0; i < exprList.size(); i++){
            if(exprList[i]->op != "__constant__"){
                if(op == "*"){
                    for(int j = 0; j < exprList.size(); j++){
                        if(j == i)  continue;
                        if(exprList[j]->op == "__constant__" && exprList[j]->constant == 0){
                            op = "__constant__";
                            constant = 0;
                            return;
                        }
                    }
                    return;
                }
                else
                    return;
            }
        }
        if(op == "+"){
            constant = exprList[0]->constant + exprList[1]->constant;
            delete exprList[1];
            delete exprList[0];
            exprList.clear();
            op = "__constant__";
        }
        else if(op == "-"){
            constant = exprList[0]->constant - exprList[1]->constant;
            delete exprList[1];
            delete exprList[0];
            exprList.clear();
            op = "__constant__";
        }
        else if(op == "*"){
            constant = exprList[0]->constant * exprList[1]->constant;
            delete exprList[1];
            delete exprList[0];
            exprList.clear();
            op = "__constant__";
        }
        else if(op == "/"){
            if(exprList[1]->constant == 0)
                throw syntaxError(0, 0x233, "Zero at denominator");
            constant = exprList[0]->constant / exprList[1]->constant;
            delete exprList[1];
            delete exprList[0];
            exprList.clear();
            op = "__constant__";
        }
        else if(op == "%"){
            if((int)exprList[0]->constant != exprList[0]->constant || (int)exprList[1]->constant != exprList[1]->constant)
                throw syntaxError(0, 0x234, "Invalid operands");
            constant = (int)exprList[0]->constant % (int)exprList[1]->constant;
            delete exprList[1];
            delete exprList[0];
            exprList.clear();
            op = "__constant__";
        }
        else if(op == "^"){
            constant = pow(exprList[0]->constant, exprList[1]->constant);
            delete exprList[1];
            delete exprList[0];
            exprList.clear();
            op = "__constant__";
        }
        else if(op == "sin"){
            constant = sin(exprList[0]->constant);
            delete exprList[0];
            exprList.clear();
            op = "__constant__";
        }
        else if(op == "cos"){
            constant = cos(exprList[0]->constant);
            delete exprList[0];
            exprList.clear();
            op = "__constant__";
        }
        else if(op == "tan"){
            constant = tan(exprList[0]->constant);
            delete exprList[0];
            exprList.clear();
            op = "__constant__";
        }
    }
}

void expr::Diff(const QVector<QString> & parameters, QString var){
    for(int i = 0; i < parameters.size(); i++)
        qDebug()<<parameters;
    qDebug()<<"diffing, var="<<var;
    qDebug()<<"expr:"<<Print(Stat::INFIX);
    qDebug()<<"op:"<<op;
    if(op == "__constant__"){
        constant = 0;
        return;
    }
    else if(op == "__variable__"){
        if(variable == var){
            qDebug()<<"meeting var";
            op = "__constant__";
            variable = "";
            constant = 1;
            return;
        }
        else
            return;
    }
    else if(op == "__function__"){
        return;
    }
    else{
        if(op == "+" || op == '-'){
            exprList[0]->Diff(parameters, var);
            exprList[1]->Diff(parameters, var);
        }
        else if(op == "*"){
            //(<expr1> * <expr2>)' = <expr1>' * <expr2> + <expr1> * <expr2>'
            expr* leftExpr;
            expr* rightExpr;
            expr* diff_1 = new expr(exprList[0]);
            expr* diff_2 = new expr(exprList[1]);
            expr* cpy_1 = new expr(exprList[0]);
            expr* cpy_2 = new expr(exprList[1]);
            delete exprList[0];
            delete exprList[1];
            diff_1->Diff(parameters, var);
            diff_2->Diff(parameters, var);
            QVector<expr*> tempList;
            tempList.push_back(diff_1);
            tempList.push_back(cpy_2);
            leftExpr = new expr("*", tempList);
            tempList.clear();
            tempList.push_back(cpy_1);
            tempList.push_back(diff_2);
            rightExpr = new expr("*", tempList);
            exprList.clear();
            exprList.push_back(leftExpr);
            exprList.push_back(rightExpr);
            op = "+";
        }
        else if(op == "/"){
            //(<expr1> / <expr2>)' = (<expr1>'*<expr2> - <expr1>*<expr2>')/(<expr>^2)
            expr* leftExpr;
            expr* rightExpr;
            expr* dividendExpr;
            expr* divisorExpr;
            expr* diff_1 = new expr(exprList[0]);
            expr* diff_2 = new expr(exprList[1]);
            expr* cpy_1 = new expr(exprList[0]);
            expr* cpy_2 = new expr(exprList[1]);
            expr* cpy_2_2 = new expr(exprList[1]);
            expr* powExpr = new expr("__constant__", 2);
            delete exprList[0];
            delete exprList[1];
            diff_1->Diff(parameters, var);
            diff_2->Diff(parameters, var);
            QVector<expr*> tempList;
            tempList.push_back(diff_1);
            tempList.push_back(cpy_2);
            leftExpr = new expr("*", tempList);
            tempList.clear();
            tempList.push_back(cpy_1);
            tempList.push_back(diff_2);
            rightExpr = new expr("*", tempList);
            tempList.clear();
            tempList.push_back(leftExpr);
            tempList.push_back(rightExpr);
            dividendExpr = new expr("-", tempList);
            tempList.clear();
            tempList.push_back(cpy_2_2);
            tempList.push_back(powExpr);
            divisorExpr = new expr("^", tempList);
            exprList.clear();
            exprList.push_back(dividendExpr);
            exprList.push_back(divisorExpr);
            op = "/";
        }
    }
}
