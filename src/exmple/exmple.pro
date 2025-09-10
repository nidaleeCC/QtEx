QT += gui core widgets
TEMPLATE = app

TARGET = exmple
INCLUDEPATH += $$PWD

# HEADERS += \
SOURCES += \
    $$PWD/main.cpp

LIBS += -L$$PWD/dll -lQtCoreEx
