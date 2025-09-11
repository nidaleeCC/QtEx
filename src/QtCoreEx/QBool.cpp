#include "QBool.h"

QBool::QBool() : m_bool(false),m_value(){}

QBool::QBool(const QVariant &value) : m_bool(true),m_value(value){}

QBool::QBool(const QBool &other) : m_bool(other.m_bool),m_value(other.m_value) {}

QBool::QBool(QBool &&other) noexcept : m_bool(other.m_bool),m_value(other.m_value) {
    other.m_bool = false; // 移动后清空原值
    other.m_value = QVariant(); // 移动后清空原值
}

QBool &QBool::operator=(QBool &&other) noexcept {
    if (this != &other) {
        m_bool = other.m_bool;
        m_value = other.m_value;
        other.m_bool = false; // 移动后清空原值
        other.m_value = QVariant(); // 移动后清空原值
    }
    return *this;
}

QBool::operator bool() const { return m_bool; }

QBool QBool::operator!() const { return QBool(!m_bool); }

QVariant QBool::error() const
{
    return m_value;
}

QString QBool::string() const
{
    return m_value.toString();
}

bool QBool::hasError() const
{
    return m_bool;
}

void QBool::reset(const QVariant &value)
{
    value.isNull() ? QBool() : QBool(value);
}

QBool &QBool::operator=(const QBool &other) {
    if (this != &other) {
        m_bool  = other.m_bool;
        m_value = other.m_value;
    }
    return *this;
}
