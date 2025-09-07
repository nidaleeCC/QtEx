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
        tcp           = 0x8,
        dump          = 0x10,
        Placeholder_2 = 0x20,
        Placeholder_3 = 0x40,
        Placeholder_4 = 0x80,
        Placeholder_5 = 0x100,
    };
}

class QDebugSink;//基类
class QDebugConsoleSink;    //控制台
class QDebugFileSink;       //文件
class QDebugUdpSink;//保留类名
class QDebugTcpSink;//保留类名
class QDebugdumpSink;//保留类名

class QTCOREX_EXPORT QDebugEx : public Singleton<QDebugEx>
{
    friend class Singleton<QDebugEx>;
    friend void ExMessageHandler(QtMsgType type, const QMessageLogContext &,const QString &msg);
    QEX_PIMPL_DECL
    QDebugEx();
public:
    /**
     * @brief initConfig 初始化日志配置
     * @param filePath   日志配置文件的文件名称
     * @param sinkFlag   sink标志，内置了 控制台 文件 udp 等输出方式。每种sink有默认的实现。修改其具体实现可以通过代码或配置文件修改
     * todo:实现部分
     */
    void initConfig(const QString& filePath,Qt::QDebugExSinkFlag sinkFlag);

    /**
     * @brief addSink  添加一个用户自定义的sink
     * @param flag     标志不能是内置的标志位,必须要大于0x100
     * @param sink     需要实现out()函数的sink类
     * todo:未实现
     */
    void addSink(QDebugSink* sink);

    /**
     * @brief sink 获取存在的sink,不存在返回nullptr
     * @param flag sink标志
     * @return
     * todo:未实现
     */
    QDebugSink* sink(int flag);


    /**
     * @brief removeSink  移除一个sink,并返回所有权
     * @param flag        sink标志
     * @return
     * todo:未实现
     */
    QDebugSink* removeSink(int flag);
};

/**
 * @brief The QDebugFormatAttr class
 * 输出的格式化属性，内置了 [No.%1] [thread:%1] [func：%1] [file:%1] [line:%1] [msg：%1] 属性
 * [msg：%1]属性不可移除,其余属性可以通过QDebugSink接口移除
 * 自定义QDebugSink要重写virtual QString format(const QMessageLogContext & context,const QString& msg);
 */
class QDebugFormatAttr
{
public:
    QDebugFormatAttr(const QString& attrName);
    virtual ~QDebugFormatAttr() = default;
    QString attrName()const;
    virtual QString format(const QMessageLogContext & context,const QString& msg);
protected:
    QString m_attrName;
};


/**
 * @brief The QDebugSink class
 * 日志输出管道基类，设计概念中参考boost库的log sink；每一个输出目的地都是一个sink。
 * sink提供基本的日志配置：格式化属性，启用开关，配置文件策略
 * sink通过继承out虚函数来确定日志输出目的地。
 * 库中提供QDebugConsoleSink QDebugFileSink两种管道，一个是控制台，一个是文件日志。
 */
class QTCOREX_EXPORT QDebugSink
{

    friend void ExMessageHandler(QtMsgType type, const QMessageLogContext &,const QString &msg);
    QEX_PIMPL_DECL
public:
    QDebugSink(Qt::QDebugExSinkFlag flag);

    virtual ~QDebugSink() = default;
    /**
     * @brief setEnable  设置是否启用管道，启用后，日志正常输出，否则不输出  todo:未实现
     * @param isEnable
     */
    void setEnable(bool isEnable);

    /**
     * @brief isEnable 返回当前的启用状态 true = 启用 todo:未实现
     * @return
     */
    bool isEnable()const;

    /**
     * @brief attrs 内置的格式化属性表,[No.%1] [func：%1] [file:%1] [line:%1] [msg：%1] 属性
     * 移除某个属性，将不会显示某个格式化序列，msg属性移除后不会输出打印消息
     * @return
     */
    QList<QSharedPointer<QDebugFormatAttr>>& attrs();

    /**
     * @brief registerConfig  注册配置文件字段  todo:未实现
     * @param key
     * @param value
     * @return
     */
    bool registerConfig(const QString& key,const QString& value);
protected:
    virtual void out(const QString& fmtMsg) = 0;
};


/**
 * @brief The QDebugConsoleSink class
 * 控制台日志管道，往控制台输出信息。内部实现用fprintf,可以在windows和lunix下使用。
 */
class QTCOREX_EXPORT QDebugConsoleSink : public QDebugSink
{
public:
    QDebugConsoleSink():QDebugSink(Qt::console){}
    virtual ~QDebugConsoleSink(){}
    virtual void out(const QString& fmtMsg)override;


};

/**
 * @brief The QDebugFileSink class
 * 文件日志管道，未实现
 */
class QTCOREX_EXPORT QDebugFileSink : public QDebugSink
{
public:
    QDebugFileSink():QDebugSink(Qt::file){}
    virtual ~QDebugFileSink(){}

};

#endif  //QDEBUGEX_H
