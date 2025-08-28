QT += core

TEMPLATE = lib      # 表示生成库
CONFIG += dll       # 生成动态库（DLL）

TARGET = QtCoreEx  # DLL

DEFINES += QTCOREX_LIBRARY

HEADERS += \
    $$PWD/QDebugEx.h\
    $$PWD/QGlobalEx.h\
    $$PWD/QDesignPattern.h

SOURCES += \
    $$PWD/QDebugEx.cpp


# 1. 中间文件（obj / moc / ui / qrc）统一放 obj 目录


# 2. 最终 DLL/LIB/PDB 输出到 bin 目录


OBJECTS_DIR = obj
contains(CONFIG,debug){
    OBJECTS_DIR = debug
}else{
    OBJECTS_DIR = release
}
MOC_DIR = $$OBJECTS_DIR/moc
RCC_DIR = $$OBJECTS_DIR/rcc
UI_DIR  = $$OBJECTS_DIR/ui
DESTDIR = $$OBJECTS_DIR/bin
