#include "QDebugEx.h"
#include <QtCore/QSharedPointer>
#include <QtCore/QThread>
#include <QtCore/QFile>
#include <QtCore/QSysInfo>
#include <QtCore/QDate>
#include <QtCore/QDir>
#ifdef Q_OS_WIN
#include <windows.h>
#include <fileapi.h>
#else
#endif

intptr_t g_mainThreadID = 0L;
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

void QDebugEx::initLog(Qt::QDebugExSinkFlags sinkFlag)
{
    g_mainThreadID = reinterpret_cast<intptr_t>(reinterpret_cast<int*>(QThread::currentThreadId()));

    if(sinkFlag & Qt::console)
        d->m_sinks << pimpl::QDebugSinkPtr(new QDebugConsoleSink);
    if(sinkFlag & Qt::file)
        d->m_sinks << pimpl::QDebugSinkPtr(new QDebugFileSink);

    if(d->m_sinks.size())
        qInstallMessageHandler(ExMessageHandler);
}

bool QDebugEx::addSink(QDebugSink *sink)
{
    bool res = false;
    pimpl::QDebugSinkPtr pSink(sink);
    if(std::find(d->m_sinks.begin(),d->m_sinks.end(),pSink) == d->m_sinks.end())
    {
        d->m_sinks << pSink;
        res = true;
    }
    return res;
}

QDebugSink* QDebugEx::sink(int flag)
{
    QDebugSink* dSink = nullptr;
    for(const pimpl::QDebugSinkPtr& pSink : qAsConst(d->m_sinks))
    {
        if(pSink->flag() == flag)
        {
            dSink = pSink.data();
            break;
        }
    }
    return dSink;
}


QDebugFormatAttr::QDebugFormatAttr(const QString& attrName)
    :m_attrName(attrName),m_newFormat(nullptr)
{

}

QString QDebugFormatAttr::attrName() const
{
    return m_attrName;
}

QString QDebugFormatAttr::format(const QMessageLogContext & context,const QString& msg)
{
    Q_UNUSED(context)
    if(m_newFormat)
        return m_newFormat(context,msg);
    return QString(m_attrName).arg(msg);
}

void QDebugFormatAttr::reset(const std::function<QString (const QMessageLogContext &, const QString &)> &newFormat)
{
    m_newFormat = newFormat;
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
        :QDebugFormatAttr("[msg：%1] ")//,m_isWin7(false)
    {
    }

    virtual QString format(const QMessageLogContext & context,const QString& msg)override
    {
        Q_UNUSED(context)
        return QDebugFormatAttr::format(context,msg);
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
        QString fmtstr;
        intptr_t tid = reinterpret_cast<intptr_t>(reinterpret_cast<int*>(QThread::currentThreadId()));
        if(g_mainThreadID && g_mainThreadID == tid)
            fmtstr = QString("(main)%1").arg(tid);
        else
            fmtstr = QString("(sub)%1").arg(tid);
        return QDebugFormatAttr::format(context,fmtstr);
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
    for(auto& attr : this->attrs())
    {
        if(attr->attrName().contains("msg")) //重写msg属性，支持颜色
        {
            attr->reset([=](const QMessageLogContext& context,const QString& msg){
                Q_UNUSED(context);
                QString fmt = isWin7 ? msg :QString("msg:\033[31m%1\033[0m").arg(msg);
                return fmt;
            });
            break;
        }
    }

}

void QDebugConsoleSink::out(const QString& fmtMsg)
{
    fprintf(stdout, "%s\n", fmtMsg.toLocal8Bit().constData());
}

void ExMessageHandler(QtMsgType type, const QMessageLogContext & context,const QString &msg)
{
    for(const QDebugEx::pimpl::QDebugSinkPtr& pSink : qAsConst(QDebugEx::getSingleton().d->m_sinks))
    {
        if(!pSink->isEnable())
            continue;
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

QEX_PIMPL_IMPORT(QDebugFileSink)
{
    QSharedPointer<QFile> m_file;
    Qt::QDebugExFileSinkMode m_mode;
    int m_reserveCount;
    pimpl(QDebugFileSink* q,const QString& filePath,Qt::QDebugExFileSinkMode mode,int reserveCount)
        :m_mode(mode),m_reserveCount(reserveCount)
    {
        int cIndex = 0;
        if(!removeLogFiles(filePath,cIndex))
        {
            q->setEnable(false);
            return;
        }

        QString fileName = filePath + "/" + QDate::currentDate().toString(Qt::ISODate) + QString("_%1_log.txt").arg(cIndex, 5, 10, QChar('0'));
        m_file = QSharedPointer<QFile>::create(fileName);

        if (!m_file->open(QIODevice::Append | QIODevice::Text))
        {
            QString str = QString("不能打开%1文件").arg(filePath);
            qDebug() << str;
            return;
        }
    }
    //只保留最新的文件
    bool removeLogFiles(const QString &path,int& index)
    {
        QDir dir(path);
        if(!dir.exists())
        {
            dir.mkdir(path);
        }
        QFileInfoList list = dir.entryInfoList(QStringList() << "*_log.txt",
                                       QDir::Files | QDir::NoDotAndDotDot);
        index = 0;
        //如果文件数量>=保留的数量 ，移除差值
        if(m_reserveCount - list.size() <= 0)
        {
            int removeCount = m_reserveCount - list.size() + 1;
            // 按创建时间升序排序（最早的排在前面）
            std::sort(list.begin(), list.end(), [](const QFileInfo &a, const QFileInfo &b) {
                return a.birthTime() < b.birthTime();
            });
            for(int i = 0; i < removeCount;i++)
            {
                const QFileInfo &info = list[i];
                if (!QFile::remove(info.absoluteFilePath())) {
                   qDebug() << "删除失败:" << info.fileName();
                }
            }
        }
        //获取最后一个文件名称提取索引
        if(list.size())
        {
            QString lastFileName = list.last().fileName();
            QStringList parts = lastFileName.split("_");
            if(parts.size() > 1)
            {
               index = parts[1].toInt() + 1;
            }
        }

        return true;
    }

    void flush()
    {
        m_file->flush();
#ifdef Q_OS_WIN
        FlushFileBuffers(reinterpret_cast<HANDLE>(m_file->handle())); // 强制落盘
#else
        ::fsync(m_file->handle()); // Unix/Linux 强制落盘
#endif
    }
};

QDebugFileSink::QDebugFileSink(Qt::QDebugExFileSinkMode mode,int reserveCount,const QString& filePath)
    :QDebugSink(Qt::file),d(this,filePath,mode,reserveCount)
{

}

QDebugFileSink::~QDebugFileSink()
{
    d->flush();
}

void QDebugFileSink::out(const QString &fmtMsg)
{
    QByteArray data = fmtMsg.toUtf8() + "\n";
    d->m_file->write(data);
    if(d->m_mode == Qt::flash)
        d->flush();
}

QString QDebugFileSink::filePath() const
{
    QString fileName;
    if(d->m_file)
        fileName = d->m_file->fileName();
    return fileName;
}

Qt::QDebugExFileSinkMode QDebugFileSink::mode() const
{
    return d->m_mode;
}

int QDebugFileSink::reserveCount() const
{
    return d->m_reserveCount;
}
