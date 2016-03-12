#include "variadic_unittest.h"
#include "variadic_unittest.moc"

#include <QDebug>
#include <QDialog>
#include <QMenu>
#include <QThread>

#include <sgi/details/type_list>
#include <sgi/details/object_type_info>
#include <sgi/details/call_function_for_object_type>

#define SGI_NO_HOSTITEM_GENERATOR
#include <sgi/GenerateItem>
#include <sgi/plugins/SGIHostItemBase.h>
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/plugins/SGIItemQt>
#include <sgi/plugins/SGIPluginImpl.h>

//#include <sgi/helpers/meta.hpp>

#include <tuple>

using namespace std;
using namespace sgi;

inline QDebug operator<< (QDebug dbg, const std::string & s)
{
    return dbg << QString::fromStdString(s);
}


void variadic_unittest::initTestCase()
{
}

void variadic_unittest::cleanupTestCase()
{
}

template<typename BaseT, typename DerivedT>
struct object_hier {
    typedef BaseT base_type;
    typedef DerivedT derived_type;
};

object_hier<QObject, tuple<QWindow, QWidget, QThread, QCoreApplication> > obj_hier_QObject;

void variadic_unittest::test_index_of()
{
    typedef details::type_list<QWindow, QWidget, QThread> ddd;

    int n;
    n = details::index_of<QThread, ddd >::value;
    QCOMPARE(n, 2);

    n = details::index_of<int, ddd >::value;
    QCOMPARE(n, -1);
}


struct counter
{
    unsigned num;
    counter() : num(0) {}

    template<typename T>
    bool operator()()
    {
        ++num;
        //std::cout << typeid(T).name();
        return true;
    }
};

void variadic_unittest::test_for_each()
{
    typedef details::type_list<QWindow, QWidget, QThread> ddd;

    counter cnt;
    details::for_each_type<ddd>(cnt);
    QCOMPARE(cnt.num, 3u);

}

void variadic_unittest::test_sizeof()
{
    typedef details::type_list<QWindow, QWidget, QThread> ddd;

    size_t s = details::type_list_size<ddd>::value;
    QCOMPARE(s, (size_t)3);

    //QCOMPARE(ddd::size, (size_t)3);

    //QCOMPARE(ddd::size, s);
}

template<>
struct object_info::object_type_info<QObject>
    : details::object_type_info_impl<QObject, details::type_list<QWidget, QThread>, QObjectCaster > {};

template<>
struct object_info::object_type_info<QWidget>
    : details::object_type_info_impl<QWidget, details::type_list<QMenu, QDialog>, QObjectCaster > {};

GENERATE_IMPL_TEMPLATE()

void variadic_unittest::test_item_qobject()
{
    SGIHostItemQt hostItem(new QObject);

    typedef details::generateSGIItemT<SGIItemQt, generateItemAcceptImpl> generateSGIItemFunctor;
    generateSGIItemFunctor func(&hostItem);
    details::call_function_for_object_type<SGIHostItemQt::ObjectType>(hostItem.object(), func);

    QVERIFY(func.wasAccepted());
    SGIItemBasePtr item = func.getItem();
    QVERIFY(item.valid());

    QCOMPARE(item->score(), 0u);
}

void variadic_unittest::test_item_qmenu()
{
    SGIHostItemQt hostItem(new QMenu);

    typedef details::generateSGIItemT<SGIItemQt, generateItemAcceptImpl> generateSGIItemFunctor;
    generateSGIItemFunctor func(&hostItem);
    details::call_function_for_object_type<SGIHostItemQt::ObjectType>(hostItem.object(), func);

    QVERIFY(func.wasAccepted());
    SGIItemBasePtr item = func.getItem();
    QVERIFY(item.valid());

    QCOMPARE(item->score(), 2u);
}

QTEST_MAIN(variadic_unittest)
