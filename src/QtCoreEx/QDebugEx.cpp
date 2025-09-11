#include "QDebugEx.h"
#include "private/QDebugEx_p.h"

QDebug operator<<(QDebug dbg, const QBool &t)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "QBool(" << t.error() << ")";
    return dbg;
}


QDebugEx::QDebugEx()
    :d(){}

QDebugEx::~QDebugEx()
{
    uninit();
}

QDebugEx *QDebugEx::instance()
{
    return Singleton<QDebugEx>::instance();
}


void QDebugEx::initLog(Qt::QDebugExSinkFlags sinkFlag)
{
    QDebugThreadFormartAttr::g_mainThreadID = reinterpret_cast<intptr_t>(reinterpret_cast<int*>(QThread::currentThreadId()));

    if(sinkFlag & Qt::console)
        d->m_sinks << pimpl::QDebugSinkPtr(new QDebugConsoleSink);
    if(sinkFlag & Qt::file)
        d->m_sinks << pimpl::QDebugSinkPtr(new QDebugFileSink);
    if(sinkFlag & Qt::udp)
        qWarning() << ("udp sink未实现");
    if(sinkFlag & Qt::tcp)
        qWarning() << ("tcp sink未实现");
    if(sinkFlag & Qt::dump)
        qWarning() << ("dump sink未实现");
    if(sinkFlag & Qt::Placeholder_2)
        qWarning() << ("Placeholder_2 sink未实现");
    if(sinkFlag & Qt::Placeholder_3)
        qWarning() << ("Placeholder_3 sink未实现");
    if(sinkFlag & Qt::Placeholder_4)
        qWarning() << ("Placeholder_4 sink未实现");
    if(sinkFlag & Qt::Placeholder_5)
        qWarning() << ("Placeholder_5 sink未实现");

    if(d->m_sinks.size())
        qInstallMessageHandler(ExMessageHandler);
}

QBool QDebugEx::addSink(QDebugSink *sink)
{
    if(!sink)
        return QBool(("sink是空指针"));
    if(!__checkIsFlag(sink->flag()))
        return QBool(("sink的标志位不满足位要求"));
    if(sink->flag() <= 0x100)
        return QBool(("sink的标志位<=0x100，是内置的标志位，不允许使用"));

    pimpl::QDebugSinkPtr pSink(sink);
    if(std::find(d->m_sinks.begin(),d->m_sinks.end(),pSink) != d->m_sinks.end())
        return QBool(("sink的标志位在QDebugEx内部已存在"));

    d->m_sinks << pSink;
    return QBool();
}

QDebugSink* QDebugEx::sink(int flag)
{
    QDebugSink* dSink = nullptr;
    auto it = std::find_if(d->m_sinks.begin(),d->m_sinks.end(),[flag](const auto& sink){return sink->flag() == flag;});
    if(it != d->m_sinks.end())
        dSink = (*it).data();
    return dSink;
}

void QDebugEx::uninit()
{
    d->clear();
    qInstallMessageHandler(0);
}

QDebugFormatAttr::QDebugFormatAttr(const QString& attrName)
    :m_attrName(attrName),m_newFormat(nullptr){}

QString QDebugFormatAttr::attrName() const
{
    return m_attrName;
}

QString QDebugFormatAttr::format(const QMessageLogContext & context,const QString& msg)
{
    return m_newFormat ? m_newFormat(context,msg) : QString(m_attrName).arg(msg);
}

void QDebugFormatAttr::reset(const std::function<QString (const QMessageLogContext &, const QString &)> &newFormat)
{
    m_newFormat = newFormat;
}

QDebugSink::QDebugSink(Qt::QDebugExSinkFlag flag)
    :d(flag){}

void QDebugSink::setEnable(bool isEnable)
{
    if(d->m_isEnable != isEnable)
        d->m_isEnable = isEnable;
}

bool QDebugSink::isEnable() const
{
    return d->m_isEnable;
}

QList<QSharedPointer<QDebugFormatAttr>>& QDebugSink::attrs()
{
    return d->m_formats;
}

bool QDebugSink::operator==(const QDebugSink &sink)
{
    return d->m_flag == sink.d->m_flag;
}

int QDebugSink::flag() const
{
    return d->m_flag;
}

QDebugConsoleSink::QDebugConsoleSink()
    :QDebugSink(Qt::file)
{
    bool isWin7 = QSysInfo::prettyProductName().contains("Windows 7");
    auto it = std::find_if(attrs().begin(),attrs().end(),[](const auto &attr){ return attr->attrName().contains("msg"); });
    if(it != this->attrs().end()) {
        (*it)->reset([=](const QMessageLogContext &, const QString &msg){
            return isWin7 ? msg : QString("msg:\033[31m%1\033[0m").arg(msg);
        });}
}

void QDebugConsoleSink::out(const QString& fmtMsg)
{
    __fprintfEx(fmtMsg);
}

void ExMessageHandler(QtMsgType type, const QMessageLogContext & context,const QString &msg)
{
    const QString utf8msg = msg.toUtf8();
    for(const QDebugEx::pimpl::QDebugSinkPtr& pSink : qAsConst(QDebugEx::instance()->d->m_sinks))
    {
        if(!pSink->isEnable())
            continue;
        switch (type)
        {
            case QtDebugMsg:{ pSink->out(pSink->d->attrToFmt(context,utf8msg)); break;}
            case QtWarningMsg:case QtCriticalMsg: case QtFatalMsg:case QtInfoMsg:
            default:{__fprintfEx(utf8msg);break;}
        }
    }
}
