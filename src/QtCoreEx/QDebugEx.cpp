#include "QDebugEx.h"
#include <QtCore/QSharedPointer>
#include <QtCore/QThread>

void ExMessageHandler(QtMsgType type, const QMessageLogContext &context,const QString &msg);

QEX_PIMPL_IMPORT(QDebugEx)
{
    using QDebugSinkPtr = QSharedPointer<QDebugSink>;
    QList<QDebugSinkPtr> m_sinks;
};

QDebugEx::QDebugEx()
    :d()
{

}

void QDebugEx::initConfig(const QString& filePath,Qt::QDebugExSinkFlag sinkFlag)
{
    Q_UNUSED(filePath)
    if(sinkFlag & static_cast<int>(Qt::console))
        d->m_sinks << pimpl::QDebugSinkPtr(new QDebugConsoleSink);

    if(d->m_sinks.size())
        qInstallMessageHandler(ExMessageHandler);
}


QDebugFormatAttr::QDebugFormatAttr(const QString& attrName)
    :m_attrName(attrName)
{

}

QString QDebugFormatAttr::attrName() const
{
    return m_attrName;
}

QString QDebugFormatAttr::format(const QMessageLogContext & context,const QString& msg)
{
    Q_UNUSED(context)
    return QString(m_attrName).arg(msg);
}



class QDebugNoFormartAttr : public QDebugFormatAttr
{
    int m_count;
public:
    QDebugNoFormartAttr()
        :QDebugFormatAttr("[No.%1] "),m_count(0)
    {

    }

    virtual QString format(const QMessageLogContext & context,const QString& msg)override
    {
        Q_UNUSED(context)
        Q_UNUSED(msg)
        return QDebugFormatAttr::format(context,QString::number(m_count++));
    }
};


class QDebugMsgFormartAttr : public QDebugFormatAttr
{
public:
    QDebugMsgFormartAttr()
        :QDebugFormatAttr("[msg：%1] ")
    {

    }

    virtual QString format(const QMessageLogContext & context,const QString& msg)override
    {
        Q_UNUSED(context)
        return QDebugFormatAttr::format(context,QString("\033[31m%1\033[0m").arg(msg));
    }
};

class QDebugFuncFormartAttr : public QDebugFormatAttr
{
public:
    QDebugFuncFormartAttr()
        :QDebugFormatAttr("[func：%1] ")
    {

    }

    virtual QString format(const QMessageLogContext & context,const QString& msg)override
    {
        Q_UNUSED(msg)
        return QDebugFormatAttr::format(context,context.function);
    }
};

class QDebugFileFormartAttr : public QDebugFormatAttr
{
public:
    QDebugFileFormartAttr()
        :QDebugFormatAttr("[file:%1] ")
    {

    }

    virtual QString format(const QMessageLogContext & context,const QString& msg)override
    {
        Q_UNUSED(msg)
        return QDebugFormatAttr::format(context,QString(context.file).split('\\').last());
    }
};

class QDebugLineFormartAttr : public QDebugFormatAttr
{
public:
    QDebugLineFormartAttr()
        :QDebugFormatAttr("[line:%1] ")
    {

    }

    virtual QString format(const QMessageLogContext & context,const QString& msg)override
    {
        Q_UNUSED(msg)
        return QDebugFormatAttr::format(context,QString::number(context.line));
    }
};

class QDebugThreadFormartAttr : public QDebugFormatAttr
{
public:
    QDebugThreadFormartAttr()
        :QDebugFormatAttr("[thread:%1] ")
    {

    }

    virtual QString format(const QMessageLogContext & context,const QString& msg)override
    {
        Q_UNUSED(context)
        Q_UNUSED(msg)
        intptr_t tid = reinterpret_cast<intptr_t>(reinterpret_cast<int*>(QThread::currentThreadId()));
        return QDebugFormatAttr::format(context,QString::number(tid));
    }
};

QEX_PIMPL_IMPORT(QDebugSink)
{
    using QDebugFormatAttrPtr = QSharedPointer<QDebugFormatAttr>;
    using QDebugFormatAttrPtrs = QList<QDebugFormatAttrPtr>;
    Qt::QDebugExSinkFlag    m_flag;
    bool                    m_isEnable;
    QDebugFormatAttrPtrs    m_formats;
    QMap<QString,QString>   m_cfgPairMap; //<key,value>

    pimpl(Qt::QDebugExSinkFlag flag)
        :m_flag(flag),m_isEnable(true),m_formats({QDebugFormatAttrPtr(new QDebugNoFormartAttr)
                                                 ,QDebugFormatAttrPtr(new QDebugThreadFormartAttr)
                                                 ,QDebugFormatAttrPtr(new QDebugFuncFormartAttr)
                                                 ,QDebugFormatAttrPtr(new QDebugFileFormartAttr)
                                                 ,QDebugFormatAttrPtr(new QDebugLineFormartAttr)
                                                 ,QDebugFormatAttrPtr(new QDebugMsgFormartAttr)
          })
    {

    }

    QString attrToFmt(const QMessageLogContext & context,const QString &msg)
    {
        QString fmt;
        for(const auto& pAttr : qAsConst(m_formats))
        {
            fmt += pAttr->format(context,msg);
        }
        return fmt;
    }
};



QDebugSink::QDebugSink(Qt::QDebugExSinkFlag flag)
    :d(flag)
{
}

QList<QSharedPointer<QDebugFormatAttr>>& QDebugSink::attrs()
{
    return d->m_formats;
}


void QDebugConsoleSink::out(const QString& fmtMsg)
{
    fprintf(stdout, "%s\n", fmtMsg.toLocal8Bit().constData());
}

void ExMessageHandler(QtMsgType type, const QMessageLogContext & context,const QString &msg)
{
    for(const QDebugEx::pimpl::QDebugSinkPtr& pSink : qAsConst(QDebugEx::getSingleton().d->m_sinks))
    {
        switch (type)
        {
            case QtDebugMsg:
            {
                pSink->out(pSink->d->attrToFmt(context,msg));
                break;
            }
            case QtWarningMsg:
            case QtCriticalMsg:
            case QtFatalMsg:
            case QtInfoMsg:
            default:
            {
                fprintf(stdout,"%s\n",msg.toLocal8Bit().constData());
                break;
            }
        }
    }
}

