#ifndef COREFUNCTION_H
#define COREFUNCTION_H

#include "func.h"
#include "myerrors.h"
#include <QString>
#include <QStack>
#include <QRegularExpression>

extern QMap<QString, QList<int>> operatorPool;
extern QMap<QString, QList<double>> variablePool;
extern QMap<QString, func*> functionPool;

namespace ExprFlag {
    extern int ACCEPT_ALL;
    extern int ACCEPT_ZERO;
    extern int ACCEPT_FUNC;
    extern int ACCEPT_KNOWN_VARIABLE;
    extern int ACCEPT_UNDEFINED_VARIABLE;
    extern int ACCEPT_VARIABLE;
    extern int ACCEPT_DIGITS;
    extern int REQUIRE_NUM;
}

namespace Expr {
    extern int NUM;
    extern int KNOWN_VARIABLE;
    extern int UNKNOWN_VARIABLE;
    extern int UNDEFINED_VARIABLE;
    extern int OPERATOR;
    extern int FUNCTION;
    extern int UNKNOWN;
}

namespace Stat{
    extern int UNDEFINED;
    extern int INFIX;
    extern int PREFIX;
    extern int EXPR;
    extern int EQUATION;
}

void ignoreSpaces(const QString& input, int & cursor);

void InitializeOperatorPool();
void InitializeVariablePool();
void InitializeFunctionPool();

/*****************************************************************/
/******************   Functions for verifying   ******************/
/*****************************************************************/

int QuickVerify(const QString & input, int cursor = 0);
QMap<QString, QList<double>> VerifyDefParameters(const QString & parList, const int & cursor = 0);
void VerifyInfixParameters(const QString & parList, int cursor = 0, int flag = ExprFlag::ACCEPT_ALL, const QMap<QString, QList<double>> & partialVar = variablePool, int listSize = 0);
int VerifyExpr(const QString & input, int cursor = 0, int flag = ExprFlag::ACCEPT_ALL, const QMap<QString, QList<double>> & partialVar = variablePool);
int VerifyPrefixExpr(const QString & input, int cursor = 0, int flag = ExprFlag::ACCEPT_ALL, const QMap<QString, QList<double>> & partialVar = variablePool);
int VerifyInfixExpr(const QString & input, int cursor, int flag = ExprFlag::ACCEPT_ALL, const QMap<QString, QList<double>> & partialVar = variablePool);

int VerifyType(const QString & input, const QMap<QString, QList<double>> & partialVar = variablePool);

/*****************************************************************/
/****************   Functions for building expr   ****************/
/*****************************************************************/

func* Build(const QString & input, int stat);
func* BuildConstant(const QString& input, int stat);
func* BuildFunction(const QString & input, int stat);
expr* BuildExpression(const QString & input, int stat);
expr* BuildInfixExpr(const QString & input, int & cursor);
expr* BuildPrefixExpr(const QString & input, int & cursor);

#endif // COREFUNCTION_H
