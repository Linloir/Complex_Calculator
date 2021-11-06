#ifndef MYERRORS_H
#define MYERRORS_H

#include <QString>

class syntaxError{
private:
    int errorPos;
    int errorCode;
    QString errorInfo;
public:
    syntaxError():errorPos(-1),errorCode(0x0),errorInfo("Unknown error."){}
    syntaxError(int pos, int code, const QString & info):errorPos(pos),errorCode(code),errorInfo(info){}
    int pos(){return errorPos;}
    int code(){return errorCode;}
    QString info(){return errorInfo;}
};


#endif // MYERRORS_H
