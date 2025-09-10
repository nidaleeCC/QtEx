#ifndef DesignPattern_H
#define DesignPattern_H

#include <algorithm> // std::find
#include <stdexcept> // std::invalid_argument
#include <vector>
#include <memory> // for smart pointer
#include <functional>
#include <QVariant>
#include <QMutex>

/**
 * @file  Composite.h
 * @brief 简易的组合设计模式，用于存储同类对象
 */

/**
 * @class Composite
   @brief 组合模式，对组合容器的操作应当继承此类获取接口
 */

QT_BEGIN_NAMESPACE

template<typename BaseType>
class Composite : public BaseType
{
protected:
    using spointer = std::shared_ptr<BaseType>;
public:
    /**
     * @brief 实例化类型
     */
    using composite_type = Composite<BaseType>;
    /**
     * @brief 模板类型的指针
     */
    using pointer = BaseType*;
public:
    /**
     *@brief 构造一个组合类
     *@param args  基类的参数列表
     *@code
     hr::Composite<QString> comp("666");  //  组合类型是QStirng
     comp += "hello";                     //  允许对组合类进行操作
     comp.add(new QString(" world"));     //  添加一个组合对象
     *@endcode
     */
    template<typename... Args>
    Composite(Args&&... args) : BaseType(std::forward<Args>(args)...) {}

    /**
     * @brief 添加一个对象
     * @param[in] child 模板类型的指针
     */
    void add(pointer child)
    {
        children_.push_back(spointer(child));
    }

    /**
     * @brief remove  移除一个对象
     * @param[in] child 模板类型的指针
     */
    void remove(pointer child)
    {
        auto it = std::find(std::begin(children_), std::end(children_), child);
        if(it == std::end(children_))
            throw std::invalid_argument("error: removed object does not a child of the parent component!");

        children_.erase(it);
    }

    ~Composite()
    {
        children_.clear();
    }
protected:
    std::vector<spointer> children_;
};

/**
 * @brief The DataProxy class  数据代理类
 * @code




 * @endcode
 */
template <typename keyType>
class DataProxy
{
    Q_DISABLE_COPY(DataProxy)
public:
/**
 * @brief 定义获取数据接口,输入参数：对数据有选择要求，可以传入值。返回值：动态类型的数据
 */
using GetDataFunc = std::function<QVariant(const QVariant&)>;
/**
 * @brief 定义设置数据接口,输入参数1：数据
 */
using SetDataFunc = std::function<void(const QVariant&,const QVariant&)>;
/**
 * @brief 定义数据读取访问接口
 */
using DataAcessInterface = std::pair<GetDataFunc,SetDataFunc>;
public:
    DataProxy() = default;
    virtual ~DataProxy() = default;

    /**
     * @brief addSubject 添加主题
     * @param [in] key        唯一标识符
     * @param [in] intf       注册的接口
     */
    void addSubject(keyType key,const DataAcessInterface& intf);
    /**
     * @brief removeSubject 移除主题
     * @param [in] key           唯一标识符
     */
    void removeSubject(keyType key);
    /**
     * @brief getData 获取数据
     * @param [in] key     唯一标识符
     * @param [in] param   允许向注册的接口传递参数
     */
    QVariant getData(keyType key,const QVariant& param = {});
    /**
     * @brief 对注册的接口设置数据
     * @param [in] key   唯一标识符
     * @param [in] data  设置的数据
     * @param [in] param 允许向注册的接口传递参数
     */
    void setData(keyType key,const QVariant& data,const QVariant& param = {});
protected:
    QHash<keyType,DataAcessInterface> m_hash;
    QMutex m_mutex;
};

template <typename keyType>
void DataProxy<keyType>::addSubject(keyType key,const DataProxy::DataAcessInterface& intf)
{

    QMutexLocker locker(&m_mutex);
    if(!m_hash.contains(key))
        m_hash.insert(key,intf);
}

template <typename keyType>
void DataProxy<keyType>::removeSubject(keyType key)
{
    QMutexLocker locker(&m_mutex);
    if(m_hash.contains(key))
        m_hash.remove(key);
}

template <typename keyType>
QVariant DataProxy<keyType>::getData(keyType key,const QVariant& param)
{
    QMutexLocker locker(&m_mutex);
    auto it  = m_hash.find(key);
    if(it != m_hash.end())
    {
        locker.unlock();
        return it.value().first(param);
    }
    return {};
}
template <typename keyType>
void DataProxy<keyType>::setData(keyType key,const QVariant& data,const QVariant& param)
{
    QMutexLocker locker(&m_mutex);
    auto it =m_hash.find(key);
    if(it!= m_hash.cend())
    {
        //m_mutex.unlock();
        locker.unlock();
        it.value().second(data,param);
    }
}

/**
 * @class Observer
 * @brief 观察者模板类,把接收者的回调函数抽象化，以及publish可以动态发布参数，要和回调函数的参数类型匹配。
 */
template <typename keyType,typename fun>
class Observer
{
protected:
    using user = QPair<QString,fun>;
    Q_DISABLE_COPY(Observer)
public:
    Observer() = default;
    virtual ~Observer() = default;
    /**
     * @brief 订阅
     * @param [in] event     唯一标识符
     * @param [in] f         订阅的函数
     * @param [in] userName  订阅的用户
     */
    void subscribe(keyType event,const fun& f,const QString& userName);

    /**
     * @brief 发布
     * @param [in] event    唯一标识符
     * @param [in] args     传递的参数列表,fun类型的参数列表参数
     */
    template <typename ...Args>
    void publish(keyType event,Args... args);

    /**
     * @brief 取消订阅,name注册的订阅全部取消
     * @param [in] name         通过subscribe注册的名称
     */
    void unSubscribe(const QString& name);
    /**
     * @brief 取消订阅,只取消某个事件
     * @param [in] event       要取消的事件
     * @param [in] name        用户名称
     */
    void unSubscribe(keyType event,const QString& name);

    /**
     * @brief 是否存在事件
     * @param [in] event     事件名称
     * @return               事件存在返回true,反之返回false
     */
    bool containsEvent(keyType event);

protected:
    //检查事件和名称是否在哈希中
    bool contains(keyType event,const QString& userName);
protected:
    QMultiHash<keyType,user> m_hash;
    QMutex m_mutex;//线程安全的唯一锁
};

template<typename keyType,typename fun>
void Observer<keyType,fun>::subscribe(keyType event, const fun &f, const QString &userName)
{
   QMutexLocker locker(&m_mutex);
   //当前事件没有相同用户的情况下可以注册
   if(!contains(event,userName))
        m_hash.insert(event,{userName,f});
}

template<typename keyType,typename fun>
void Observer<keyType,fun>::unSubscribe(const QString &name)
{
   QMutexLocker locker(&m_mutex);
   auto keys = m_hash.keys();
   for(auto key : keys)
        unSubscribe(key,name);
}

template<typename keyType,typename fun>
void Observer<keyType,fun>::unSubscribe(keyType event, const QString &name)
{
   QMutexLocker locker(&m_mutex);
   if(m_hash.contains(event))
   {
        auto it = m_hash.find(event);
        while (it != m_hash.end() && it.key() == event)
        {
            if(it.value().first == name)
                it = m_hash.erase(it);
            else
                ++it;
        }
   }
}

template<typename keyType,typename fun>
bool Observer<keyType,fun>::containsEvent(keyType event)
{
   return m_hash.contains(event);
}

template<typename keyType,typename fun>
bool Observer<keyType,fun>::contains(keyType event, const QString &userName)
{
   if(m_hash.contains(event))
   {
        QList<user> users = m_hash.values(event);
        for(const user& u : users)
        {
            if(u.first == userName)
                return true;
        }
   }
   return false;
}

template <typename keyType,typename Fun>
template <typename... Args>
void Observer<keyType,Fun>::publish(keyType event, Args... args)
{
//    QMutexLocker locker(&m_mutex);
    if(m_hash.contains(event))
    {
        QList<user> users = m_hash.values(event);
        for(const user& u : users)
            u.second(args...);
    }
}


/**
 * @file Pimplptr.h
 * @brief piml设计模式
 */
/**
 * @brief Pimpl(Pointer to implementation) 是一种减少代码依赖和编译时间的C++编程技巧，
 * 其基本思想是将一个外部可见类(visible class)的实现细节（一般是所有私有的非虚成员）放在一个单独的实现类(implementation class)中，
 * 而在可见类中通过一个私有指针来间接访问该实现类。
 * @details example
 * @code
#include <QWidget>
#include "designPattern/Pimplptr.h"

//头文件声明
class MyWidget : public QWidget
{
//    struct pimpl;
//    hr::Pimplptr<pimpl> d;
    PIMPL_DECL   // PIMPL_DECL 等价于上述2个写法,在私有部分声明
public:
    MyWidget(QWidget* parent = nullptr);

    //私有化成员都隐藏在cpp的pimpl类中
};
----------------------------------------------------------------------------------------------------------------------------------------------------
//源文件定义
#include "MyWidget.h"

//struct MyWidget::pimpl
PIMPL_IMPORT(MyWidget) // PIMPL_IMPORT等价于上述注释写法
{
    QColor color;
    QString text;
    QSize   size;
    MyWidget*  q;  //对于视图对象的pimpl,都是基于派生控件(QWidget),因此可以在MyWidget构造之前调用基类函数。所以可以在pipml中使用主对象

    pimpl(MyWidget* q):q(q)
    {
        //对q进行初始化，切记不要调用MyWidget的自身函数,该构造在MyWidget构造初始化列表调用,调用MyWidget的函数，但是MyWidget还没初始化完成，UB行为。
        q->resize(200,400);
        q->setlayout(new QLayout());
    }
};

class MyWidget::MyWidget(QWidget* parent)
    :QWidget(parent),d(this)  //实例化hr::Pimplptr<pimpl>,传递自身参数，让pimpl负责初始化
{
}
 * @endcode
 */
template <typename T>
class Pimplptr {
    std::unique_ptr<T,void(*)(T*)> data;
public:
    template <typename... Args>
    Pimplptr(Args&&... args):data(new T(std::forward<Args>(args)...),[](T* ptr){delete ptr;}){}
    ~Pimplptr() {}


    Pimplptr(Pimplptr&& o):data(new T(std::move(*o))){}
    Pimplptr& operator=(Pimplptr&& o)      { **this = std::move(*o); return *this; }

    //2025-9-9修改，MSVC对智能指针的拷贝被构造语义严格检查，删除了拷贝
    Pimplptr(Pimplptr const& o) = delete;
    Pimplptr& operator=(Pimplptr const& o) = delete;

    T      & operator*()        { return *data; }
    T const& operator*()  const { return *data; }
    T      * operator->()       { return &**this; }
    T const* operator->() const { return &**this; }

    friend inline void swap(Pimplptr& lhs, Pimplptr& rhs)
    { using std::swap; swap(*lhs, *rhs); }
};

/**
 * @brief 可以实例化带参构造的单例
 * @code
    //作为基类使用的方式，需要声明基类友元.因为单例模式的设计要求是构造私有化，基类访问派生类的构造，声明友元
    class test : public hr::Singleton<test>
    {
        test() = default;
        friend class hr::Singleton<test>;
    public:
        int value();
    };
    //使用
    test::getSingleton().value();
 * @endcode

   @code
   //创建带参构造的单例
    class Person : public hr::Signletion<Person>
    {
        int age;
        QString name;
        friend class hr::Signletion<Person>;
        Person(int age,const QString& name):age(age),name(name){}
    public:
        int age()const { return age;};
        QString name()const{ return name;};
    };
    //使用
    auto& ref = Person::getSingletion(8,"xiaoMing"); //首次调用单例,会传递参数初始化构造
    qDebug() << ref.age() << ref.name();
   @endcode

 */
template <typename Derived>
class Singleton
{
public:
    using Singleton_T = Singleton<Derived>;
    /**
     * @brief 获取单例对象的静态函数
     * @param[in] args 模板类型的构造如果有带参构造,允许传参构造
     * @return 单例对象
     */
    template<typename... Args>
    static Derived& getSingleton(Args&&... args) /*noexcept(std::is_nothrow_default_constructible<Derived>::value)*/
    {
        static Derived instance{std::forward<Args>(args)...};
        return instance;
    }
    template<typename... Args>
    static Derived* instance(Args&&... args) /*noexcept(std::is_nothrow_default_constructible<Derived>::value)*/
    {
        return &(getSingleton(std::forward<Args>(args)...));
    }

protected:
    Singleton() = default;
    virtual ~Singleton() = default;
    Q_DISABLE_COPY_MOVE(Singleton)
private:
};

/**
 * @brief PIMPL的声明宏,查看示例用法
 */
#define QEX_PIMPL_DECL \
struct pimpl; \
Pimplptr<pimpl> d;

/**
 * @brief PIMPL的导入宏,查看示例用法
 */
#define QEX_PIMPL_IMPORT(classname) \
struct classname::pimpl


QT_END_NAMESPACE

#endif // qDesignPattern
