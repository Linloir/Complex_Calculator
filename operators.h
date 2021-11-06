#ifndef OPERATORS_H
#define OPERATORS_H

#include <QString>
#include <QRegularExpression>

class func;

class operators
{
private:
    //The type of operator. 0 for op, 1 for function
    int type;
    //The number of expressions operated by this operator
    int size;
    //The value of the operator
    func* function;
    QString val;
public:
    operators();
    operators(int t, int s, func* f = nullptr):type(t),size(s),function(f){}
    operators(int t, int s, const QString & v):type(t),size(s),function(nullptr),val(v){}
    void setSize(int _size){size = _size;}
    void setType(int _type){type = _type;}
    void setVal(func* _function){function = _function;}
    void setVal(const QString & _val){val = _val;}
    int getType(){return type;}
    int getSize(){return size;}
    func* getFunc(){return function;}
    QString getVal(){return val;}
};

#endif // OPERATORS_H
