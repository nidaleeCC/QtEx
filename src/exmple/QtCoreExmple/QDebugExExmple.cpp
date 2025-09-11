#include "QDebugExExmple.h"

void exmple::QDebugEx_exmple_initLog_console()
{
    QDebugEx::instance()->initLog();
    for(int i = 0; i < 10; i++)
        qDebug() << i; //控制台打印格式化日志
    QDebugEx::instance()->uninit();
}

void exmple::QDebugEx_exmple_initLog_file()
{
    QDebugEx::instance()->initLog(Qt::file);
    for(int i = 0; i < 10; i++)
        qDebug() << i; //文件保存日志
    QDebugEx::instance()->uninit();
}

void exmple::QDebugEx_exmple_initLog_consoleAndFile()
{
    QDebugEx::instance()->initLog(Qt::console | Qt::file);
    for(int i = 0; i < 10; i++)
        qDebug() << i;
    QDebugEx::instance()->uninit();
}

void exmple::QDebugEx_exmple_initLog_boundaryTest()
{
    QDebugEx::instance()->initLog(Qt::udp | Qt::dump | Qt::Placeholder_2);
    for(int i = 0; i < 10; i++)
        qDebug() << i;
    QDebugEx::instance()->uninit();
}

class QDebugSink_test : public QDebugSink
{
public:
    QDebugSink_test():QDebugSink(Qt::file){}
virtual void out(const QString& ){}
};

class QDebugSink_test1 : public QDebugSink
{
public:
    QDebugSink_test1():QDebugSink((Qt::QDebugExSinkFlag)17){}
virtual void out(const QString& ){}
};
class QDebugSink_test2 : public QDebugSink
{
public:
    QDebugSink_test2():QDebugSink((Qt::QDebugExSinkFlag)1024){}
virtual void out(const QString& ){}
};

void exmple::QDebugEx_exmple_addSink()
{
    QDebugEx* pDbgEx = QDebugEx::instance();
    pDbgEx->initLog();
    QBool res;
    res = pDbgEx->addSink(nullptr);         //空指针
    if(res)
        qDebug() << res;
    res = pDbgEx->addSink(new QDebugSink_test);   //内部存在的标志
    if(res)
        qDebug() << res;
    res = pDbgEx->addSink(new QDebugSink_test1);  //非用户标志
    if(res)
        qDebug() << res;
    res = pDbgEx->addSink(new QDebugSink_test2);  //用户标志
    if(res)
        qDebug() << res;

   QDebugEx::instance()->uninit();
}

void exmple::QDebugEx_exmple_sink()
{

    QDebugEx* pDbgEx = QDebugEx::instance();
    pDbgEx->initLog();
    if(pDbgEx->sink(Qt::file))
    {
        qDebug() << "file sink";
    }
    if(!pDbgEx->sink(666))
    {
        qDebug() << "没有sink";
    }
   QDebugEx::instance()->uninit();
}
