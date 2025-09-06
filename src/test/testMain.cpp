#include "QDebugEx.h"

int main(int argc,char** argv)
{
    QDebugEx::getSingleton().initConfig("aaa",Qt::console);

    for(int i = 0; i < 10;i++)
        qDebug() << i;
    return 0;
}
