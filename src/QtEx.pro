TEMPLATE = subdirs

CONFIG += c++17

SUBDIRS += \
    QtCoreEx

App.depends = QtCoreEx   # 确保先编译QtCoreEx

