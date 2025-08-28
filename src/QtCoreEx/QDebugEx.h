#ifndef QDEBUGEX_H
#define QDEBUGEX_H
#include <QtCore/QDebug>
#include <QGlobalEx.h>
#include "QDesignPattern.h"

namespace Qt{
    enum QDebugExSinkFlag
    {
        console       = 0x1,
        file          = 0x2,
        udp           = 0x4,
        Placeholder_1 = 0x8,
        Placeholder_2 = 0x10,
        Placeholder_3 = 0x20,
        Placeholder_4 = 0x40,
        Placeholder_5 = 0x80,
        Placeholder_6 = 0x100,
    };
}

class QDebugSink;//基类
class QDebugConsoleSink;    //控制台
class QDebugFileSink;       //文件
class QDebugUdpSink;//保留类名

class QTCOREX_EXPORT QDebugEx : public Singleton<QDebugEx>
{
    friend class Singleton<QDebugEx>;
    QEX_PIMPL_DECL
public:
    /**
     * @brief initConfig 初始化日志配置
     * @param filePath   日志配置文件的文件名称
     * @param sinkFlag   sink标志，内置了 控制台 文件 udp 等输出方式。每种sink有默认的实现。修改其具体实现可以通过代码或配置文件修改
     */
    void initConfig(const QString& filePath,Qt::QDebugExSinkFlag sinkFlag);

    /**
     * @brief addSink  添加一个用户自定义的sink
     * @param flag     标志不能是内置的标志位,必须要大于0x100
     * @param sink     需要实现out()函数的sink类
     */
    void addSink(int flag,QDebugSink* sink);

    /**
     * @brief sink 获取存在的sink,不存在返回nullptr
     * @param flag sink标志
     * @return
     */
    QDebugSink* sink(int flag);


    /**
     * @brief removeSink  移除一个sink,并返回所有权
     * @param flag        sink标志
     * @return
     */
    QDebugSink* removeSink(int flag);
};

/* QDebugSink */

class QTCOREX_EXPORT QDebugSink
{
public:
    virtual ~QDebugSink() = default;

    void setEnable(bool isEnable);

    bool isEnable()const;

    void setFormat(const QString& fmt);

    QString getFormat()const;

    bool registerConfig(const QString& key,const QString& value);
protected:
    virtual void out(const QVariant& data) = 0;
};



class QTCOREX_EXPORT QDebugConsoleSink : public QDebugSink
{
public:
    QDebugConsoleSink():QDebugSink(){}
    virtual ~QDebugConsoleSink(){}
};


class QTCOREX_EXPORT QDebugFileSink : public QDebugSink
{
public:
    QDebugFileSink():QDebugSink(){}
    virtual ~QDebugFileSink(){}

};

#endif  //QDEBUGEX_H
