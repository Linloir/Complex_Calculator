#ifndef EXPR_H
#define EXPR_H



#include <QMap>
#include <QVector>
#include <QString>


class expr
{
private:
    int constraintFlag;
    QString op;
    QVector<expr*> exprList;
    double constant;
    QString variable;
    func* function;
public:
    expr();
    expr(const QString & _op, const double & _c);
    expr(const QString & _op, const QString & _var);
    expr(const QString & _op, const QVector<expr*> & _exprList, func* _function);
    expr(const QString & _op, const QVector<expr*> & _exprList);
    QString Op(){return op;}
    void Create(const QString&);
    QString Print(int flag);
    double Calc();
    void Merge();
};

#endif // EXPR_H
