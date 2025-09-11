QT += gui core widgets
TEMPLATE = app

TARGET = exmple
INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/QtCoreExmple/QDebugExExmple.h
SOURCES += \
    $$PWD/QtCoreExmple/QDebugExExmple.cpp\
    $$PWD/main.cpp

LIBS += -L$$PWD/dll -lQtCoreEx
