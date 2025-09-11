#include "QtCoreExmple/QDebugExExmple.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

int main(int argc,char** argv)
{
    QApplication app(argc,argv);
    exmple::QDebugEx_exmple_decl();
    QWidget w;
    w.show();
    return app.exec();
}
