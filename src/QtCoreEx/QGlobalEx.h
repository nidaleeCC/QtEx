#ifndef QGLOBAL_EX_H
#define QGLOBAL_EX_H
#include <QtCore/qglobal.h>
/**
 * @brief 定义QTCOREX_LIBRARY则添加导出符号
 */
#if defined(NOT_QTCOREX_LIBRARY)
#  define QTCOREX_EXPORT
#elif defined(QTCOREX_LIBRARY)
#  define QTCOREX_EXPORT Q_DECL_EXPORT
#else
#  define QTCOREX_EXPORT Q_DECL_IMPORT
#endif
#endif //QGLOBAL_EX_H

