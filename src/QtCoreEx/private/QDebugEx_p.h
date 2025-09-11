#ifndef QDEBUGEX_P_H
#define QDEBUGEX_P_H
#include "../QDebugEx.h"
#include <QtCore/QThread>
#include <QtCore/QFile>
#include <QtCore/QSharedPointer>
#include <QtCore/QSysInfo>
#include <QtCore/QDate>
#include <QtCore/QDir>
#ifdef Q_OS_WIN
#include <windows.h>
#include <fileapi.h>
#else
#endif


void ExMessageHandler(QtMsgType type, const QMessageLogContext &context,const QString &msg);

QEX_PIMPL_IMPORT(QDebugEx)
{
    using QDebugSinkPtr = QSharedPointer<QDebugSink>;
    QList<QDebugSinkPtr> m_sinks;

    void clear()
    {
        m_sinks.clear();
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

    pimpl(Qt::QDebugExSinkFlag flag);
    QString attrToFmt(const QMessageLogContext & context,const QString &msg);
};


QEX_PIMPL_IMPORT(QDebugFileSink)
{
    QSharedPointer<QFile> m_file;
    Qt::QDebugExFileSinkMode m_mode;
    int m_reserveCount;

    pimpl(QDebugFileSink* q,const QString& filePath,Qt::QDebugExFileSinkMode mode,int reserveCount);
    //只保留最新的文件
    bool removeLogFiles(const QString &path,int& index);
    void flush();
};

class QDebugNoFormartAttr : public QDebugFormatAttr
{
    int m_count;
public:
    QDebugNoFormartAttr();
    virtual QString format(const QMessageLogContext & context,const QString& msg)override;
};

class QDebugMsgFormartAttr : public QDebugFormatAttr
{
public:
    QDebugMsgFormartAttr();
    virtual QString format(const QMessageLogContext & context,const QString& msg)override;
};

class QDebugFuncFormartAttr : public QDebugFormatAttr
{
public:
    QDebugFuncFormartAttr();
    virtual QString format(const QMessageLogContext & context,const QString& msg)override;
};

class QDebugFileFormartAttr : public QDebugFormatAttr
{
public:
    QDebugFileFormartAttr();
    virtual QString format(const QMessageLogContext & context,const QString& msg)override;
};

class QDebugLineFormartAttr : public QDebugFormatAttr
{
public:
    QDebugLineFormartAttr();
    virtual QString format(const QMessageLogContext & context,const QString& msg)override;
};

class QDebugThreadFormartAttr : public QDebugFormatAttr
{
public:
    QDebugThreadFormartAttr();
    virtual QString format(const QMessageLogContext & context,const QString& msg)override;
    static intptr_t g_mainThreadID;
};


inline void __fprintfEx(const QString& msg)
{
    fprintf(stdout,"%s\n",msg.toLocal8Bit().constData());
    fflush(stdout);
}

inline bool __checkIsFlag(int f)
{
    return f > 0 && (f & (f - 1)) == 0; //检查2的幂算法  f & (n - 1) == 0
}
#endif
