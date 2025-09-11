#ifndef QDEBUGEX_H
#define QDEBUGEX_H
#include <QtCore/QDebug>
#include "QGlobalEx.h"
#include "QDesignPattern.h"
#include "QBool.h"
#include <QtCore/QCoreApplication>

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
    Q_DECLARE_FLAGS(QDebugExSinkFlags, QDebugExSinkFlag)
    enum QDebugExFileSinkMode
    {
        cache,  //缓存模式，效率更高，但不是立即写入文件
        flash,  //刷新模式，效率比缓存模式低，但是是立即写入到文件
    };
}
Q_DECLARE_OPERATORS_FOR_FLAGS(Qt::QDebugExSinkFlags)

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
    ~QDebugEx();

    static QDebugEx* instance();
    /**
     * @brief initLog 初始化日志配置
     * @param sinkFlag   sink标志，内置了 控制台 文件 udp 等输出方式。每种sink有默认的实现。修改其具体实现可以通过代码或配置文件修改
     * todo:目前支持控制台
     */
    void initLog(Qt::QDebugExSinkFlags sinkFlag = Qt::console);

    /**
     * @brief addSink  添加一个用户自定义的sink,传递的是new出来的对象,内部管理所有权，用户不需要释放
     * @param sink     sink标志不能是内置的标志位,必须要大于0x100,还要满足位要求
     * @return 添加成功返回true，添加失败返回false. 当sink标志和内部存储的sink标志相同，不允许添加
     */
    QBool addSink(QDebugSink* sink);

    /**
     * @brief sink 获取存在的sink,不存在返回nullptr
     * @param flag sink标志
     * @return
     */
    QDebugSink* sink(int flag);

    /**
     * @brief 清理资源，不主动调用，也会在析构时调用。
     */
    void uninit();
};

/**
 * @brief The QDebugFormatAttr class
 * 输出的格式化属性，内置了 [No.%1] [thread:%1] [func：%1] [file:%1] [line:%1] [msg：%1] 属性
 * [msg：%1]属性不可移除,其余属性可以通过QDebugSink接口移除
 * 自定义QDebugSink要重写virtual QString format(const QMessageLogContext & context,const QString& msg);
 */
class QTCOREX_EXPORT QDebugFormatAttr
{
public:
    QDebugFormatAttr(const QString& attrName);
    virtual ~QDebugFormatAttr() = default;
    QString attrName()const;
    virtual QString format(const QMessageLogContext & context,const QString& msg);

    void reset(const std::function<QString(const QMessageLogContext & context,const QString& msg)>& newFormat);
protected:
    QString m_attrName;
    std::function<QString(const QMessageLogContext & context,const QString& msg)> m_newFormat;
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
     * @brief setEnable  设置是否启用管道，启用后，日志正常输出，否则不输出
     * @param isEnable
     */
    void setEnable(bool isEnable);

    /**
     * @brief isEnable 返回当前的启用状态 true = 启用
     * @return
     */
    bool isEnable()const;

    /**
     * @brief attrs 内置的格式化属性表,[No.%1] [func：%1] [file:%1] [line:%1] [msg：%1] 属性
     * 移除某个属性，将不会显示某个格式化序列，msg属性移除后不会输出打印消息
     * @return
     */
    QList<QSharedPointer<QDebugFormatAttr>>& attrs();

    bool operator==(const QDebugSink& sink);

    int flag()const;
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
    QDebugConsoleSink();
    virtual ~QDebugConsoleSink(){}
    virtual void out(const QString& fmtMsg)override;
};

/**
 * @brief The QDebugFileSink class
 *
 */
class QTCOREX_EXPORT QDebugFileSink : public QDebugSink
{
    QEX_PIMPL_DECL
    virtual void out(const QString& fmtMsg)override;
public:
    /**
     * @brief QDebugFileSink
     * @param filePath      日志文件存储的路径，不需要传文件名称，名称是内部决定。
     * @param mode          写入模式
     * @param reserveCount  文件保留数量，避免日志文件持续生成，只保留最新数量的日志文件
     * @brief 如果文件检查失败，会打印错误信息。否则正常输出
     * todo:
     */
    QDebugFileSink(Qt::QDebugExFileSinkMode mode = Qt::cache
                  ,int reserveCount = 1
                  ,const QString& filePath = QCoreApplication::applicationDirPath() + "/log"
                  );
    virtual ~QDebugFileSink();
    /**
     * @brief filePath
     * @return 返回文件存储路径
     */
    QString filePath()const;

    /**
     * @brief mode
     * @return 返回写入模式
     */
    Qt::QDebugExFileSinkMode mode()const;

    /**
     * @brief reserveCount
     * @return 返回保留文件的数量。
     */
    int reserveCount()const;
};


//内部类型的流支持
QTCOREX_EXPORT QDebug operator<<(QDebug dbg, const QBool &t);
#endif  //QDEBUGEX_H
