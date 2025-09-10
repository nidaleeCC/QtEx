#include "QDebugEx.h"
#include <QtCore/QCoreApplication>
int main(int argc,char** argv)
{
    QCoreApplication app(argc,argv);
    QDebugEx::getSingleton().initLog();
    QDebugFileSink* fSink = new QDebugFileSink(Qt::flash,2);
    QDebugEx::getSingleton().addSink(fSink);
    for(int i = 0; i < 10;i++)
        qDebug() << i;

    return app.exec();
}
