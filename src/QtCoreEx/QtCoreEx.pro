QT += core

EX_TEXT = 0
message("build mode: $$EX_TEXT")


TEMPLATE = lib      # 表示生成库
CONFIG += dll       # 生成动态库（DLL）
CONFIG += debug
QMAKE_CXXFLAGS_DEBUG += -g


TARGET = QtCoreEx  # DLL
INCLUDEPATH += $$PWD

DEFINES += QTCOREX_LIBRARY

HEADERS += \
    $$PWD/private/QDebugEx_p.h\
    $$PWD/QDebugEx.h\
    $$PWD/QBool.h\
    $$PWD/QGlobalEx.h\
    $$PWD/QDesignPattern.h

SOURCES += \
    $$PWD/private/QDebugEx_p.cpp\
    $$PWD/QBool.cpp\
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



# 拷贝 DLL
dlls.files = $$OUT_PWD/debug/bin/*.dll
dlls.path  = $$PWD/../exmple/dll
# 拷贝头文件
headers.files = $$PWD/*.h
headers.path  = $$PWD/../exmple/QtCoreEx
# 注册安装目标
INSTALLS += dlls headers

message($$dlls.files)
message($$dlls.path)
message($$headers.files)
message($$headers.path)

