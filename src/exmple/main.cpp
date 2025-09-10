#include "QtCoreEx/QDebugEx.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
int main(int argc,char** argv)
{
    QApplication app(argc,argv);
    QDebugEx::instance()->initLog();
    QDebugFileSink* fSink = new QDebugFileSink(Qt::flash,2);
    QDebugEx::instance()->addSink(fSink);
    for(int i = 0; i < 10;i++)
        qDebug() << i;
    QWidget w;
    w.show();
    return app.exec();
}
