#ifndef QT_BOOL_H
#define QT_BOOL_H
#include "QGlobalEx.h"
#include <QtCore/QVariant>
#include <QtCore/QString>
/**
 * @brief QBool用于在返回类型时，可以携带一个数据，可以是字符串，可以是一个QVariant数据
 * 规则约束：0是正常，没有数据。 非0是异常，携带数据
 */
class QTCOREX_EXPORT QBool
{
public:
    QBool();
    QBool(const QVariant& value);
    QBool(const QBool& other);
    QBool& operator=(const QBool& other);
    QBool(QBool&& other) noexcept;
    QBool& operator=(QBool&& other) noexcept;
    operator bool() const;
    QBool operator!() const;
    QVariant error()const;
    QString string()const; //尝试把variant转为QString，转换失败返回空字符串
    bool hasError()const;
    void reset(const QVariant& value = QVariant()); //如果传递的是默认参，内部会设为false。反之会设为true
protected:
    bool     m_bool;
    QVariant m_value;
};
#endif  //QT_BOOL_H
