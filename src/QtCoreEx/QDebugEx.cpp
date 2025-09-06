#include "QDebugEx.h"
#include <QtCore/QSharedPointer>


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
    qInstallMessageHandler(ExMessageHandler);

    if(sinkFlag | static_cast<int>(Qt::console))
        d->m_sinks << pimpl::QDebugSinkPtr(new QDebugConsoleSink);
}


QDebugFormatAttr::QDebugFormatAttr(const QString& attrName)
    :m_attrName(attrName),m_fun(nullptr)
{

}

QString QDebugFormatAttr::format(const QString& msg)
{
    if(m_fun)
        return m_fun(msg);
    return QString(m_attrName).arg(msg);
}

void QDebugFormatAttr::reset(const std::function<QString(const QString&)>& formatFun)
{
    m_fun = formatFun;
}


class QDebugNoFormartAttr : public QDebugFormatAttr
{
    int m_count;
public:
    QDebugNoFormartAttr()
        :QDebugFormatAttr("[No.%1] "),m_count(0)
    {

    }

    virtual QString format(const QString& msg)override
    {
        if(m_fun)
            return m_fun(msg);
        return QDebugFormatAttr::format(QString::number(m_count++));
    }
};


class QDebugMsgFormartAttr : public QDebugFormatAttr
{
public:
    QDebugMsgFormartAttr()
        :QDebugFormatAttr("[msg：%1] ")
    {

    }

    virtual QString format(const QString& msg)override
    {
        if(m_fun)
            return m_fun(msg);
        return QDebugFormatAttr::format(msg);
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
                                                 ,QDebugFormatAttrPtr(new QDebugMsgFormartAttr)
          })
    {

    }

    QString attrToFmt(const QMessageLogContext & context,const QString &msg)
    {
        QString fmt;
        for(const auto& pAttr : m_formats)
        {
            fmt += pAttr->format(msg);
        }
        return fmt;
    }
};



QDebugSink::QDebugSink(Qt::QDebugExSinkFlag flag)
    :d(flag)
{
}



void QDebugConsoleSink::out(const QString& fmtMsg)
{
    fprintf(stdout, "%s\n", fmtMsg.toLocal8Bit().constData());
}

void ExMessageHandler(QtMsgType type, const QMessageLogContext & context,const QString &msg)
{
    for(const QDebugEx::pimpl::QDebugSinkPtr& pSink : QDebugEx::getSingleton().d->m_sinks)
    {
        switch (type)
        {
            case QtDebugMsg:
            {
                pSink->out(pSink->d->attrToFmt(context,msg));
                break;
            }
        }
    }
}

