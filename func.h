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
    QVector<QString> parameters;
    QMap<QString, QList<double>> partialVar;
    int size;
    expr* expression;
public:
    func();
    func(const func* f);
    func(const QString & _name, expr* _exp);
    func(const QString & _name, const QVector<QString> & _par, const QMap<QString, QList<double>> & _var, expr* _exp);
    int Size(){return size;}
    expr* Expr(){return expression;}
    QString Name(){return name;}
    QString VarName(int i){return i < parameters.size() ? parameters[i] : "";}
    int IndexOf(QString var){return parameters.indexOf(var);}
    QString Print(int flag);
    void Assign(QString var, expr* exp);
    void Assign(QVector<expr*> exprList);
    void Merge();
    void Diff();
};

class expr
{
private:
    int constraintFlag;
    QString op;
    double constant;
    QString variable;
    func* function;
public:
    QVector<expr*> exprList;
    expr();
    expr(const expr* e);
    expr(const QString & _op, const double & _c);
    expr(const QString & _op, const QString & _var);
    expr(const QString & _op, const QVector<expr*> & _exprList, func* _function);
    expr(const QString & _op, const QVector<expr*> & _exprList);
    QString Op(){return op;}
    double Constant(){return op == "__constant__" ? constant : 0;}
    QString Variable(){return op == "__variable__" ? variable : "";}
    void Create(const QString&);
    QString Print(int flag);
    void Assign(QString var, expr* exp);
    void Assign(double key, QString var);
    void Merge(const QMap<QString, QVector<double>>& partialVar);
    void Diff(const QVector<QString> & parameters, QString var);
};

#endif // FUNC_H
