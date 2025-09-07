#include "QDebugEx.h"

int main(int argc,char** argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);
    QDebugEx::getSingleton().initConfig("aaa",Qt::console);

    for(int i = 0; i < 10;i++)
        qDebug() << i;
    return 0;
}
