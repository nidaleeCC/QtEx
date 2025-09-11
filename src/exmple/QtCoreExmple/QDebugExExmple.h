#ifndef QDEBUG_EX_EXMPLE_H
#define QDEBUG_EX_EXMPLE_H
#include "QtCoreEx/QDebugEx.h"
/**
 * QDebugEx测试用例
 */

namespace exmple
{

/* QDebugEx::initLog  接口测试 */

//! 预期结果：可以在控制台打印日志。msg在非win7环境下显示颜色字体
void QDebugEx_exmple_initLog_console();

//! 预期结果：可以在文件存储日志。
void QDebugEx_exmple_initLog_file();

//! 预期结果：可以控制台和文件都保存日志
void QDebugEx_exmple_initLog_consoleAndFile();

//! 预期结果：边界外的参数要在控制台打印提示信息
void QDebugEx_exmple_initLog_boundaryTest();


/* QDebugEx::addSink  接口测试 */

//! 预期结果：能正确自定义sink，能识别空指针，能识别不正确flag
void QDebugEx_exmple_addSink();


/* QDebugEx::sink  接口测试 */

//! 预期结果：内部有sink返回sink，没有返回nullptr
void QDebugEx_exmple_sink();

inline void QDebugEx_exmple_decl()
{
    QDebugEx_exmple_sink();
}

}//namespace exmple

#endif
