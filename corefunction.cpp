#include "corefunction.h"

//Global variables' pool is defined here.
QMap<QString, QList<int>> operatorPool;
QMap<QString, QList<double>> variablePool;
QMap<QString, func*> functionPool;

namespace ExprFlag {
    int ACCEPT_ALL = 0b11111111;
    int ACCEPT_ZERO = 0b00000001;
    int ACCEPT_FUNC = 0b00000010;
    int ACCEPT_KNOWN_VARIABLE = 0b00000100;
    int ACCEPT_UNDEFINED_VARIABLE = 0b00001000;
    int ACCEPT_VARIABLE = 0b00001100;
    int ACCEPT_DIGITS = 0b00010000;
    int REQUIRE_NUM = 0b00100000;
}

namespace Expr {
    int NUM = 0b0110001;
    int KNOWN_VARIABLE = 0b0110101;
    int UNKNOWN_VARIABLE = 0b0000100;
    int UNDEFINED_VARIABLE = 0b00001000;
    int OPERATOR = 0b0100000;
    int FUNCTION = 0b1000000;
    int UNKNOWN = 0b10000000;
}

namespace Stat {
    int UNDEFINED = 0b0000000;
    int INFIX = 0b0000100;
    int PREFIX = 0b0000010;
    int EXPR = 0b0010000;
    int EQUATION = 0b0001000;
    int CONSTANT = 0b0100000;
    int FUNCTION = 0b1000000;
}

void ignoreSpaces(const QString& input, int & cursor){
    while(cursor < input.length() && input[cursor] == ' ')
        cursor++;
}

void InitializeOperatorPool(){
    //This function is used to initialize operator pool
    operatorPool.insert("+", {2,0});
    operatorPool.insert("-", {2,0});
    operatorPool.insert("*", {2,1});
    operatorPool.insert("/", {2,1});
    operatorPool.insert("%", {2,1});
    operatorPool.insert("^", {2,2});
    operatorPool.insert("sin", {1,3});
    operatorPool.insert("cos", {1,3});
    operatorPool.insert("tan", {1,3});
    operatorPool.insert("'", {2,5});
}

void InitializeVariablePool(){
    //This function is used to initialize
    variablePool.insert("PI", {1,3.14159265358});
}

void InitializeFunctionPool(){
    //functionPool.insert("sin", 1);
}

int QuickVerify(const QString& input, int cursor){
    int stat = Stat::UNDEFINED;
    int eqPos = input.indexOf('=');
    if(eqPos == -1){
        //Situation of one expression
        stat |= Stat::EXPR;
        stat |= VerifyExpr(input, 0, ExprFlag::ACCEPT_ALL);
    }
    else{
        //Situation of assignment
        stat |= Stat::EQUATION;
        //Check expr 1
        //Create regular expression for legal variable name
        QRegularExpression varMask("\\s*[a-zA-Z]([_a-zA-Z0-9])*\\s*");
        //capture result
        QRegularExpressionMatch matchResult = varMask.match(input, cursor);
        if(!matchResult.hasMatch() || matchResult.capturedStart() != cursor || matchResult.captured().length() == 0){
            //Invalid variable name
            throw syntaxError(0, 0x1, "Invalid variable name");
        }
        else if(operatorPool.contains(matchResult.captured().remove(' ')) || functionPool.contains(matchResult.captured().remove(' '))){
            //Redefinition
            throw syntaxError(0, 0xA, "Redefinition here");
        }
        //move the cursor
        cursor += matchResult.captured().length();
        //ignore spaces
        ignoreSpaces(input, cursor);
        if(input[cursor] == '='){
            //Expression 1 is a variable
            //verify right expression
            stat |= Stat::CONSTANT;
            QMap<QString, QVector<double>> partialVar = variablePool;
            if(!partialVar.contains(matchResult.captured().remove(' ')))
                partialVar.insert(matchResult.captured().remove(' '), {0,0});
            stat |= VerifyExpr(input, eqPos + 1, ExprFlag::ACCEPT_ALL & (~ExprFlag::ACCEPT_UNDEFINED_VARIABLE), partialVar);
        }
        else if(input[cursor] == '('){
            cursor++;
            //Expression 1 is a function
            //Pair for ')'
            int rBrac = input.indexOf(')', cursor);
            if(cursor == rBrac){
                //Empty parameter list
                throw syntaxError(cursor, 0x2, "Empty parameter list");
            }
            else if(rBrac == -1 || rBrac > eqPos){
                //Missing bracket
                throw syntaxError(eqPos - 1, 0xF, "Missing bracket");
            }
            else{
                int p = rBrac+1;
                ignoreSpaces(input, p);
                if(input[p] != '=')
                    throw syntaxError(p, 0x4, "Redundant expressions");
                //Verify parameter table and construct partial variable table
                QMap<QString, QList<double>> partialVar = VerifyDefParameters(input.mid(cursor, rBrac - cursor), cursor);
                for(int i = 0; i < variablePool.size(); i++){
                    if(!partialVar.contains(variablePool.keys().at(i)))
                        partialVar.insert(variablePool.keys().at(i), {0,0});
                }
                //verify right expression
                stat |= Stat::FUNCTION;
                stat |= VerifyExpr(input, eqPos + 1, ExprFlag::ACCEPT_ALL & (~ExprFlag::ACCEPT_UNDEFINED_VARIABLE), partialVar);
            }
        }
        else{
            throw syntaxError(cursor, 0x3, "Can't assign value to an expression");
        }
    }
    return stat;
}

QMap<QString, QList<double>> VerifyDefParameters(const QString & parList, const int & cursor){
    //In this case, only variables are allowed
    int ptr = cursor;
    while(ptr < parList.size() && parList[ptr] == ' '){ptr++;}
    QStringList params = parList.split(',');
    QMap<QString, QList<double>> partialVar;
    for(int i = 0; i < params.size(); i++){
        while(ptr < parList.size() && parList[ptr] == ' '){ptr++;}
        params[i].remove(' ');
        int type = VerifyType(params[i]);
        if(type == Expr::KNOWN_VARIABLE || type == Expr::UNKNOWN_VARIABLE || type == Expr::UNDEFINED_VARIABLE)
            partialVar.insert(params[i], {0, 0});
        else
            throw syntaxError(ptr, 0xFFF, "Not a parameter");
        //qDebug() << "inserted:" << params[i];
        ptr += params[i].length() + 1;

    }
    return partialVar;
}

void VerifyInfixParameters(const QString & parList, int cursor, int flag, const QMap<QString, QList<double>> & partialVar, int listSize){
    //This functions is used to verify whether the parameter list of a function is legal.
    QStringList params = parList.split(',');
    if(listSize > 0 && listSize != params.size()){
        if(listSize < params.size())
            throw syntaxError(parList.length() + cursor, 0xF000004, "Missing parameter");
        else
            throw syntaxError(parList.length() + cursor, 0xF000005, "Redundant parameter");
    }
    for(int i = 0; i < params.size(); i++){
        VerifyInfixExpr(params[i], 0, ExprFlag::ACCEPT_VARIABLE | flag, partialVar);
    }
}

int VerifyExpr(const QString& input, int cursor, int flag, const QMap<QString, QList<double>> & partialVar){
    int stat = Stat::UNDEFINED;

    ignoreSpaces(input, cursor);
    //Judge whether the input is prefix or infix
    QRegularExpression numberPrefixMask("(-?[0-9])|[a-zA-Z]|\\(");
    QRegularExpressionMatch result = numberPrefixMask.match(input, cursor);
    //qDebug() << result.capturedStart();
    //qDebug() << input;
    if(!result.hasMatch() || result.capturedStart() != cursor || result.captured().length() == 0){
        //not starting with a number or a variable
        //Prefix expression
        stat |= Stat::PREFIX;
        int end = VerifyPrefixExpr(input, cursor, flag, partialVar);
        ignoreSpaces(input, end);
        if(end != input.length()){
            throw syntaxError(end, 0x4, "redundant expressions");
        }
    }
    else{
        stat |= Stat::INFIX;
        int end = VerifyInfixExpr(input, cursor, flag, partialVar);
        ignoreSpaces(input, end);
        if(end != input.length()){
            throw syntaxError(end, 0x4, "redundant expressions");
        }
    }
    return stat;
}

int VerifyPrefixExpr(const QString& input, int cursor, int flag, const QMap<QString, QList<double>> & partialVar){
    if(cursor == input.length()){
        throw syntaxError(cursor, 0xF0, "Missing input");
    }

    //ignore space
    ignoreSpaces(input, cursor);
    if(cursor == input.length()){
        //empty expression
        throw syntaxError(cursor - 1, 0xFF, "Empty expression");
    }

    //Expect a number or a variable or an operator
    //get the next section
    QString section;
    int next = input.indexOf(' ', cursor);
    if(next == -1)
        section = input.mid(cursor);
    else
        section = input.mid(cursor, next - cursor);

    //judge
    int type = VerifyType(section, partialVar);
    if((type & flag) == 0){
        //Wrong type
        throw syntaxError(cursor, 0xF000000 | flag, "Wrong type here.");
    }

    next = next == -1 ? input.length() : next;
    if(type == Expr::NUM || type == Expr::KNOWN_VARIABLE || type == Expr::UNKNOWN_VARIABLE)
        return next;
    else if(type == Expr::OPERATOR){
        for(int i = 0; i < operatorPool.find(section).value()[0]; i++){
            next = VerifyPrefixExpr(input, next, flag, partialVar);
        }
        return next;
    }
    else if(type == Expr::FUNCTION){
        for(int i = 0; i < functionPool.find(section).value()->Size(); i++){
            next = VerifyPrefixExpr(input, next, flag, partialVar);
        }
        return next;
    }
    else
        throw syntaxError(cursor, 0x10, "Unknown input");
}

int VerifyInfixExpr(const QString & input, int cursor, int flag, const QMap<QString, QList<double>> & partialVar){
    if(cursor == input.length()){
        throw syntaxError(cursor, 0xF0, "Missing input");
    }

    ignoreSpaces(input, cursor);
    if(cursor == input.length())
        throw syntaxError(cursor - 1, 0xF0, "Missing input");

    if(input[cursor] == '('){
        //situation: (<expr1>) [op] [expr2]
        cursor++;
        int end = VerifyInfixExpr(input, cursor, flag, partialVar);
        if(end == cursor)
            throw syntaxError(cursor, 0xF1, "Empty expression");
        cursor = end;
        //Pair for ')'
        if(cursor == input.length() || input[cursor] != ')'){
            //Missing bracket
            throw syntaxError(cursor, 0xF, "Missing bracket");
        }
        cursor++;
    }
    else{
        //situation: <element> [op] [expr2]
        QRegularExpression number("(-?\\d+)(\\.\\d+)?");
        QRegularExpression variable("[a-zA-Z][_a-zA-Z0-9]*");
        QRegularExpressionMatch isNum;
        QRegularExpressionMatch isVar;
        //match for numbers
        isNum = number.match(input, cursor);
        isVar = variable.match(input, cursor);
        if(isNum.hasMatch() && isNum.capturedStart() == cursor && isNum.capturedLength() != 0){
            //situation: <number> [op] [expr2]
            cursor += isNum.capturedLength();
        }
        else if(isVar.hasMatch() && isVar.capturedStart() == cursor && isVar.capturedLength() != 0){
            //situation: <var / func> [op] [expr2]
            int type = VerifyType(input.mid(cursor, isVar.capturedLength()), partialVar);
            if(type == Expr::KNOWN_VARIABLE || type == Expr::UNKNOWN_VARIABLE)
                cursor += isVar.capturedLength();
            else if(type == Expr::UNDEFINED_VARIABLE && (flag & ExprFlag::ACCEPT_UNDEFINED_VARIABLE) == 0)
                throw syntaxError(cursor, 0x100, "Undefined variable");
            else if(type == Expr::UNDEFINED_VARIABLE)
                cursor += isVar.capturedLength();
            else if(type == Expr::UNKNOWN)
                throw syntaxError(cursor, 0xF000001, "Unexpected input");
            else if(type == Expr::FUNCTION){
                //<funcName>([parameter list])
                cursor += isVar.capturedLength();
                ignoreSpaces(input, cursor);
                if(cursor == input.length())
                    throw syntaxError(cursor, 0xF0, "Missing input");
                if(input[cursor] != '(')
                    throw syntaxError(cursor, 0xF, "Missing bracket");
                cursor++;
                //Pair for ')'
                ignoreSpaces(input, cursor);
                QStringList params = input.mid(cursor).split(',');
                int count = 0;
                for(count = 0; count < params.size(); ){
                    int end = VerifyInfixExpr(input, cursor, flag, partialVar);
                    if(end == cursor)
                        throw syntaxError(cursor, 0xF1, "Empty expression");
                    ignoreSpaces(input, end);
                    if(end >= input.length())
                        throw syntaxError(input.length(), 0xF, "Missing bracket");
                    cursor = end;
                    count++;
                    if(input[cursor] == ')')
                        break;
                    if(input[cursor] == ',')
                        cursor++;
                }
                if(count < functionPool.value(isVar.captured().remove(' '))->Size())
                    throw syntaxError(cursor, 0xB0, "Missing parameter");
                else if(count > functionPool.value(isVar.captured().remove(' '))->Size())
                    throw syntaxError(cursor, 0xB1, "Redundant parameter");
                if(input[cursor] != ')')
                    throw syntaxError(input.length(), 0xF, "Missing bracket");
                cursor++;
            }
            else if(type == Expr::OPERATOR){
                if(operatorPool.find(isVar.captured()).value()[0] != 1){
                    //if not an one dimension operator
                    throw syntaxError(cursor, 0xF000001, "Unexpected input");
                }
                else{
                    cursor += isVar.capturedLength();
                    ignoreSpaces(input, cursor);
                    if(cursor == input.length())
                        throw syntaxError(cursor, 0xF0, "Missing input");
                    if(input[cursor] != '(')
                        throw syntaxError(cursor, 0xF, "Missing bracket");
                    cursor++;
                    int end = VerifyInfixExpr(input, cursor, flag, partialVar);
                    if(end == cursor)
                        throw syntaxError(cursor, 0xF1, "Empty expression");
                    cursor = end;
                    //Pair for ')'
                    if(cursor == input.length() || input[cursor] != ')'){
                        //Missing bracket
                        throw syntaxError(cursor, 0xF, "Missing bracket");
                    }
                    cursor++;
                }
            }
        }
        else
            return cursor;
    }

    ignoreSpaces(input, cursor);
    if(cursor == input.length())
        return cursor;

    //check for op
    if(operatorPool.contains(input[cursor]))
        cursor++;
    else{
        QRegularExpression variable("[a-zA-Z][_a-zA-Z0-9]*");
        QRegularExpressionMatch isVar;
        isVar = variable.match(input, cursor);
        if(isVar.hasMatch() && isVar.capturedStart() == cursor && isVar.capturedLength() != 0){
            int type = VerifyType(input.mid(cursor, isVar.capturedLength()), partialVar);
            if(type != Expr::OPERATOR || operatorPool.find(isVar.captured()).value()[0] != 2)
                throw syntaxError(cursor, 0xF000002, "Missing operator. Expect an operator here.");
            cursor += isVar.capturedLength();
        }
        else
            return cursor;
    }

    ignoreSpaces(input, cursor);
    if(cursor == input.length())
        throw syntaxError(cursor, 0xF0, "Missing input");

    //check for expression 2
    cursor = VerifyInfixExpr(input, cursor, flag, partialVar);

    ignoreSpaces(input, cursor);
    return cursor;
}

int VerifyType(const QString & input, const QMap<QString, QList<double>> & partialVar){
    //qDebug() << input;
    QRegularExpression number("^(-?\\d+)(\\.\\d+)?$");
    QRegularExpression variable("^[a-zA-Z][_a-zA-Z0-9]*$");
    QRegularExpressionMatch result;
    result = number.match(input);
    //qDebug()<<"number capt:"<< result.captured();
    //qDebug()<<"input length"<<input.length()<<"capt length"<<result.captured().length();
    if(result.hasMatch() && result.captured().length() == input.length()){
        //qDebug()<<"isNum";
        return Expr::NUM;
    }
    result = variable.match(input);
    if(result.hasMatch() && result.captured().length() == input.length()){
        if(functionPool.contains(input))
            return Expr::FUNCTION;
        else if(operatorPool.contains(input))
            return Expr::OPERATOR;
        else if(partialVar.contains(input)){
            if(partialVar.find(input).value()[0] == 0)
                return Expr::UNKNOWN_VARIABLE;
            else
                return Expr::KNOWN_VARIABLE;
        }
        else if(variablePool.contains(input)){
            if(variablePool.find(input).value()[0] == 0)
                return Expr::UNKNOWN_VARIABLE;
            else
                return Expr::KNOWN_VARIABLE;
        }
        else
            return Expr::UNDEFINED_VARIABLE;
    }
    else if(operatorPool.contains(input))
        return Expr::OPERATOR;
    else
        return Expr::UNKNOWN;
}

func* Build(const QString & input, int stat){
    func* newFunc = nullptr;
    if(stat & Stat::EQUATION){
        if(stat & Stat::FUNCTION){
            newFunc = BuildFunction(input, stat);
            functionPool.insert(newFunc->Name(), newFunc);
        }
        else{
            newFunc = BuildConstant(input, stat);
        }
    }
    else{
        expr* newExpr = BuildExpression(input, stat);
        newFunc = new func("__TEMP__", variablePool.keys().toVector(), variablePool, newExpr);
    }
    return newFunc;
}

func* BuildConstant(const QString & input, int stat){
    QString variableName;
    expr* expression;

    //Read variable name
    //Create regular expression for legal variable name
    QRegularExpression varMask("\\s*[a-zA-Z]([_a-zA-Z0-9])*\\s*");
    //capture result
    QRegularExpressionMatch matchResult = varMask.match(input);
    variableName = matchResult.captured().remove(' ');

    expression = BuildExpression(input.mid(input.indexOf('=') + 1), stat);
    expression->Merge(variablePool);

    if(expression->Op() == "__variable__" && expression->Variable() == variableName){
        //Definition of global variable
        qDebug()<<"Defined" << variableName;
        variablePool.remove(variableName);
        variablePool.insert(variableName, {0,0});
    }
    else if(expression->Op() != "__constant__")
        throw syntaxError(input.indexOf('=') + 1, 0x30, "Not a constant");
    else{
        variablePool.remove(variableName);
        variablePool.insert(variableName, {1, expression->Constant()});
        qDebug()<<"Inserted"<<variableName<<","<<expression->Constant();
    }

    func* newFunc = new func("__TEMP__", expression);
    return newFunc;
}

func* BuildFunction(const QString& input, int stat){
    QString funcName;
    expr* expression;

    //Read function name
    //Create regular expression for legal variable name
    QRegularExpression varMask("\\s*[a-zA-Z]([_a-zA-Z0-9])*\\s*");
    //capture result
    QRegularExpressionMatch matchResult = varMask.match(input);
    funcName = matchResult.captured().remove(' ');

    //Read parameter list
    int s = input.indexOf('(') + 1;
    int e = input.indexOf(')', s);
    QMap<QString, QList<double>> partialVar = VerifyDefParameters(input.mid(s, e - s), s);
    QStringList params = input.mid(s, e - s).split(',');

    //Read expression
    expression = BuildExpression(input.mid(input.indexOf('=') + 1), stat);

    func* newFunc = new func(funcName, params, partialVar, expression);
    return newFunc;
}

expr* BuildExpression(const QString & input, int stat){
    int cursor = 0;
    if(stat & Stat::INFIX)
        return BuildInfixExpr(input, cursor);
    else
        return BuildPrefixExpr(input, cursor);
}

expr* BuildInfixExpr(const QString& input, int& cursor){
    QStack<QString> opStack;
    QStack<expr*> exprStack;
    while(cursor != input.length()){
        ignoreSpaces(input, cursor);
        if(input[cursor] == '('){
            opStack.push_back("(");
            cursor++;
            continue;
        }
        if(input[cursor] == ')'){
            qDebug()<<")";

            while(!opStack.isEmpty() && opStack.top() != "("){
                QVector<expr*> exprList;
                for(int i = 0; i < operatorPool.value(opStack.top())[0]; i++){
                    exprList.insert(0, exprStack.pop());
                }
                expr* newExpr = new expr(opStack.pop(), exprList);
                exprStack.push_back(newExpr);
            }
            if(opStack.isEmpty()){
                //no (
                cursor++;
                break;
            }
            else if(opStack.top() == "(")
                opStack.pop();
            cursor++;
            continue;
        }
        if(operatorPool.contains(input[cursor]) && cursor != 0 && input[cursor-1] != '('){
            if(opStack.isEmpty() || !operatorPool.contains(opStack.top()) || operatorPool.value(input[cursor])[1] > operatorPool.value(opStack.top())[1])
                opStack.push_back(input[cursor]);
            else{
                qDebug()<<"priority less than";
                while(!opStack.isEmpty() && operatorPool.contains(opStack.top()) && operatorPool.value(input[cursor])[1] <= operatorPool.value(opStack.top())[1]){
                    QVector<expr*> exprList;
                    for(int i = 0; i < operatorPool.value(opStack.top())[0]; i++){
                        exprList.insert(0, exprStack.pop());
                    }
                    expr* newExpr = new expr(opStack.pop(), exprList);
                    exprStack.push_back(newExpr);
                }
                opStack.push_back(input[cursor]);
            }
            cursor++;
            continue;
        }
        QRegularExpression number("(-?\\d+)(\\.\\d+)?");
        QRegularExpression variable("[a-zA-Z][_a-zA-Z0-9]*");
        QRegularExpressionMatch isNum;
        QRegularExpressionMatch isVar;
        isNum = number.match(input, cursor);
        isVar = variable.match(input, cursor);
        if(isNum.hasMatch() && isNum.capturedStart() == cursor){
            //found number
            expr* newExpr = new expr("__constant__", isNum.captured().toDouble());
            exprStack.push_back(newExpr);
            cursor += isNum.capturedLength();
            continue;
        }
        else if(isVar.hasMatch() && isVar.capturedStart() == cursor){
            //found variable
            if(operatorPool.contains(isVar.captured())){
                //operator
                if(opStack.isEmpty() || operatorPool.value(input[cursor])[1] > operatorPool.value(opStack.top())[1])
                    opStack.push_back(isVar.captured());
                else{
                    while(!opStack.isEmpty() || operatorPool.value(input[cursor])[1] <= operatorPool.value(opStack.top())[1]){
                        QVector<expr*> exprList;
                        for(int i = 0; i < operatorPool.value(opStack.top())[0]; i++){
                            exprList.insert(0, exprStack.pop());
                        }
                        expr* newExpr = new expr(opStack.pop(), exprList);
                        exprStack.push_back(newExpr);
                    }
                    opStack.push_back(isVar.captured());
                }
                cursor += isVar.capturedLength();
                continue;
            }
            else if(functionPool.contains(isVar.captured())){
                //function
                func* function = functionPool.value(isVar.captured());
                cursor += isVar.capturedLength();
                ignoreSpaces(input, cursor);
                cursor++;
                ignoreSpaces(input, cursor);
                QVector<expr*> exprList;
                for(int i = 0; i < function->Size(); i++){
                    if(input[cursor] == ',')
                        cursor++;
                    ignoreSpaces(input, cursor);
                    expr* newExpr = BuildInfixExpr(input, cursor);
                    qDebug()<<newExpr->Print(Stat::INFIX);
                    ignoreSpaces(input, cursor);
                    if(cursor == input.length()){
                        exprList.push_back(newExpr);
                        break;
                    }
                    exprList.push_back(newExpr);
                }

                func* assigned = new func(function);
                assigned->Assign(exprList);
                exprStack.push_back(assigned->Expr());
                delete assigned;
                continue;
            }
            else{
                //variable
                expr* newExpr = new expr("__variable__", isVar.captured());
                exprStack.push_back(newExpr);
                cursor += isVar.capturedLength();
                continue;
            }
        }
        else
            break;
    }
    while(!opStack.isEmpty() && operatorPool.contains(opStack.top())){
        QVector<expr*> exprList;
        for(int i = 0; i < operatorPool.value(opStack.top())[0]; i++){
            exprList.insert(0, exprStack.pop());
        }
        expr* newExpr = new expr(opStack.pop(), exprList);
        exprStack.push_back(newExpr);
    }
    return exprStack.top();
}

expr* BuildPrefixExpr(const QString& input, int& cursor){
    ignoreSpaces(input, cursor);

    expr* thisExpr;

    QString section;
    int next = input.indexOf(' ', cursor);
    if(next == -1)
        section = input.mid(cursor);
    else
        section = input.mid(cursor, next - cursor);

    int type = VerifyType(section);
    next = next == -1 ? input.length() : next;
    if(type == Expr::NUM){
        thisExpr = new expr("__constant__", section.toDouble());
        cursor = next;
    }
    else if(type == Expr::KNOWN_VARIABLE){
        if(variablePool.contains(section) && variablePool.value(section)[0] == 1){
            thisExpr = new expr("__constant__", variablePool.value(section)[1]);
            cursor = next;
        }
        else{
            thisExpr = new expr("__variable__", section);
            cursor = next;
        }
    }
    else if(type == Expr::UNKNOWN_VARIABLE || type == Expr::UNDEFINED_VARIABLE){
        thisExpr = new expr("__variable__", section);
        cursor = next;
    }
    else if(type == Expr::OPERATOR){
        QVector<expr*> exprList;
        for(int i = 0; i < operatorPool.find(section).value()[0]; i++){
            exprList.push_back(BuildPrefixExpr(input, next));
        }
        thisExpr = new expr(section, exprList);
        cursor = next;
    }
    else{
        QVector<expr*> exprList;
        for(int i = 0; i < functionPool.find(section).value()->Size(); i++){
            exprList.push_back(BuildPrefixExpr(input, next));
        }
        func* assigned = new func(functionPool.find(section).value());
        assigned->Assign(exprList);
        //thisExpr = new expr("__function__", exprList, functionPool.find(section).value());
        thisExpr = new expr(assigned->Expr());
        delete assigned;
        cursor = next;
    }

    return thisExpr;
}
