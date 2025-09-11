#include "QDebugEx_p.h"
intptr_t QDebugThreadFormartAttr::g_mainThreadID = 0;

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

QDebugThreadFormartAttr::QDebugThreadFormartAttr()
    :QDebugFormatAttr("[thread:%1] ")
{

}

QString QDebugThreadFormartAttr::format(const QMessageLogContext &context, const QString &msg)
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

QDebugNoFormartAttr::QDebugNoFormartAttr()
    :QDebugFormatAttr("[No.%1] "),m_count(0)
{
}

QString QDebugNoFormartAttr::format(const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)
    Q_UNUSED(msg)
    return QDebugFormatAttr::format(context,QString::number(m_count++));
}

QDebugMsgFormartAttr::QDebugMsgFormartAttr()
    :QDebugFormatAttr("[msg：%1] ")//,m_isWin7(false)
{
}

QString QDebugMsgFormartAttr::format(const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)
    return QDebugFormatAttr::format(context,msg);
}

QDebugFuncFormartAttr::QDebugFuncFormartAttr()
    :QDebugFormatAttr("[func：%1] ")
{

}

QString QDebugFuncFormartAttr::format(const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(msg)
    return QDebugFormatAttr::format(context,context.function);
}

QDebugFileFormartAttr::QDebugFileFormartAttr()
    :QDebugFormatAttr("[file:%1] ")
{

}

QString QDebugFileFormartAttr::format(const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(msg)
    return QDebugFormatAttr::format(context,QString(context.file).split('\\').last());
}

QDebugLineFormartAttr::QDebugLineFormartAttr()
    :QDebugFormatAttr("[line:%1] ")
{

}

QString QDebugLineFormartAttr::format(const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(msg)
    return QDebugFormatAttr::format(context,QString::number(context.line));
}

QDebugSink::pimpl::pimpl(Qt::QDebugExSinkFlag flag)
    :m_flag(flag),m_isEnable(true),m_formats({QDebugFormatAttrPtr(new QDebugNoFormartAttr)
          ,QDebugFormatAttrPtr(new QDebugThreadFormartAttr)
          ,QDebugFormatAttrPtr(new QDebugFuncFormartAttr)
          ,QDebugFormatAttrPtr(new QDebugFileFormartAttr)
          ,QDebugFormatAttrPtr(new QDebugLineFormartAttr)
          ,QDebugFormatAttrPtr(new QDebugMsgFormartAttr)
      })
{

}

QString QDebugSink::pimpl::attrToFmt(const QMessageLogContext &context, const QString &msg)
{
    QString fmt;
    for(const auto& pAttr : qAsConst(m_formats))
    {
        fmt += pAttr->format(context,msg);
    }
    return fmt;
}

QDebugFileSink::pimpl::pimpl(QDebugFileSink *q, const QString &filePath, Qt::QDebugExFileSinkMode mode, int reserveCount)
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
        QString str = QStringLiteral("不能打开%1文件").arg(filePath);
                      qWarning() << str;
        return;
    }
}

bool QDebugFileSink::pimpl::removeLogFiles(const QString &path, int &index)
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
                    qWarning() << QStringLiteral("删除失败:") << info.fileName();
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

void QDebugFileSink::pimpl::flush()
{
    m_file->flush();
#ifdef Q_OS_WIN
    FlushFileBuffers(reinterpret_cast<HANDLE>(m_file->handle())); // 强制落盘
#else
    ::fsync(m_file->handle()); // Unix/Linux 强制落盘
#endif
}
