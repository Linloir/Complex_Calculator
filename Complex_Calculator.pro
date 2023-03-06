QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    corefunction.cpp \
    customhint.cpp \
    custominputbox.cpp \
    displayitem.cpp \
    func.cpp \
    inputindicator.cpp \
    main.cpp \
    mainwindow.cpp \
    myerrors.cpp \
    scrollareacustom.cpp \
    scrollcontainer.cpp \
    scrollindicator.cpp \
    scrolllistcontainer.cpp

HEADERS += \
    corefunction.h \
    customhint.h \
    custominputbox.h \
    displayitem.h \
    func.h \
    inputindicator.h \
    mainwindow.h \
    myerrors.h \
    scrollareacustom.h \
    scrollcontainer.h \
    scrollindicator.h \
    scrolllistcontainer.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
