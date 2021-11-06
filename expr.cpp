#include "expr.h"

expr::expr()
{

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
    if(flag & Stat::INFIX){
        if(op == "__constant__"){
            return QString::asprintf("%g", constant);
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
                    if(operatorPool.value(exprList[0]->Op())[1] < operatorPool.value(op)[1]){
                        res.append("(");
                        res.append(exprList[0]->Print(Stat::INFIX));
                        res.append(")");
                    }
                    else
                        res.append(exprList[0]->Print(Stat::INFIX));
                    res.append(op);
                    if(operatorPool.value(exprList[1]->Op())[1] < operatorPool.value(op)[1]){
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
                QString res;
                res.append(function->Name())
            }
        }
    }
    else{

    }
}
