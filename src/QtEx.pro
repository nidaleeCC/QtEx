TEMPLATE = subdirs

CONFIG += c++17

SUBDIRS +=   \
    QtCoreEx \
    QtNetworkEx

App.depends = QtCoreEx   # 确保先编译QtCoreEx
App.depends = QtNetworkEx







