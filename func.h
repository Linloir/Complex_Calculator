#ifndef FUNC_H
#define FUNC_H

#include <QString>
#include <QMap>
#include <QVector>

class func;
class expr;

extern QMap<QString, QList<int>> operatorPool;
extern QMap<QString, QList<double>> variablePool;
extern QMap<QString, func*> functionPool;

namespace Stat{
    extern int UNDEFINED;
    extern int INFIX;
    extern int PREFIX;
    extern int EXPR;
    extern int EQUATION;
}

class func
{
private:
    QString name;
    QMap<QString, QList<double>> partialVar;
    int size;
    expr* expression;
public:
    func();
    func(const func* f);
    func(const QString & _name, const QVector<QString> & _var, expr* _exp);
    int Size(){return size;}
    expr* Expr(){return expression;}
    QString Name(){return name;}
    QString VarName(int i){return i < partialVar.size() ? partialVar.keys()[i] : "";}
    QString Print(int flag);
    double Calc();
    void Assign(QString var, expr* exp);
    void Assign(QVector<expr*> exprList);
    void Merge();
};

class expr
{
public:
    int constraintFlag;
    QString op;
    QVector<expr*> exprList;
    double constant;
    QString variable;
    func* function;
public:
    expr();
    expr(const expr* e);
    expr(const QString & _op, const double & _c);
    expr(const QString & _op, const QString & _var);
    expr(const QString & _op, const QVector<expr*> & _exprList, func* _function);
    expr(const QString & _op, const QVector<expr*> & _exprList);
    QString Op(){return op;}
    void Create(const QString&);
    QString Print(int flag);
    double Calc();
    void Assign(QString var, expr* exp);
    void Merge();
};

#endif // FUNC_H
