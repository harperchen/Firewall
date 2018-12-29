TEMPLATE = app

QT += qml quick

CONFIG += c++11

SOURCES += main.cpp \
    qrule.cpp \
    qfiltertable.cpp \
    qnat.cpp \
    qnattable.cpp \
    qlinktable.cpp \
    chrdev.cpp \
    utils.cpp \
    logwatcher.cpp \
    qlinkget.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += qrule.h \
    rule.h \
    qfiltertable.h \
    qnat.h \
    qnattable.h \
    qlinktable.h \
    chrdev.h \
    utils.h \
    logwatcher.h \
    qlinkget.h

LIBS += -lpthread
