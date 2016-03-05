#include "variadic_unittest.h"
#include "variadic_unittest.moc"

#include <QDebug>
#include <QDialog>
#include <QMenu>
#include <QThread>

#include <sgi/details/type_list>
#include <sgi/details/object_type_info>

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

#if 0
template <template<typename, typename> class CallFunctionT, template<typename> class Params, typename TypeList>
struct v2_generateItemImpl
{
    static bool generate(const SGIHostItemBase * object, SGIItemBasePtr & item)
    {
        bool ret = false;
        for(size_t n = 0; !ret && n < meta::size<TypeList>(); ++n)
        {
            constexpr size_t _n = n;
            typedef Loki::Int2Type<_n> s;
            typedef typename meta::at<TypeList, s> HostItemType;
            if(const HostItemType * hostitem = object->as<HostItemType>())
            {
                typedef generateSGIItemT<HostItemType, Params> generateSGIItemFunctor;
                generateSGIItemFunctor func(hostitem);
                typedef typename HostItemType::ObjectType ObjectType;
                CallFunctionT<ObjectType, generateSGIItemFunctor>(hostitem->object(), func);
                ret = func.wasAccepted();
                if(ret)
                    item = func.getItem();
            }
        }
        return ret;
    }
};
#endif

template<typename BaseT, typename DerivedT>
struct object_hier {
    typedef BaseT base_type;
    typedef DerivedT derived_type;
};

object_hier<QObject, tuple<QWindow, QWidget, QThread, QCoreApplication> > obj_hier_QObject;

#if 0
template< typename TL, size_t NO>
void dump_element()
{
    //template<size_t N> struct TL::type<N>;
    using X = typename TL::type;
    typedef X element_type;
    //typedef typename TL::type<typename NO> element_type;
    std::cout << typeid(element_type).name() << std::endl;
}
#endif


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
    void operator()()
    {
        ++num;
        //std::cout << typeid(T).name();
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

SGI_CALL_FUNCTION_FOR_OBJECT_TEMPLATE()

template<>
struct object_type_info<QWidget>
    : details::object_type_info_impl<QWidget, details::type_list<QMenu, QDialog>, QObjectCaster > {};

template<typename BaseType>
struct call_function_for_object_type {
    template<typename Functor>
    struct call_proxy
    {
        BaseType * _obj;
        Functor & _op;
        call_proxy(BaseType * obj, Functor & op) : _obj(obj), _op(op) {}
        template<typename T>
        void operator()()
        {
            typedef typename object_type_info<BaseType>::object_caster object_caster;
            T * obj = object_caster::template cast<T>(_obj);
            if(obj) {
                _op.decend(obj);
                call_function_for_object_type<T>(reinterpret_cast<T*>(_obj), _op);
                _op.ascend(obj);
            }
        }
    };

    template<typename Functor>
    call_function_for_object_type(BaseType * object, Functor & op) {

        if(op.canAccept(object))
            op.accept(object);

        typedef typename object_type_info<BaseType>::derived_types derived_types;
        call_proxy<Functor> proxy(object, op);
        details::for_each_type<derived_types>(proxy);
        //call_function_for_object_and_derivedT<BaseType, typename DerivedClassesT<BaseType>::DerivedClasses, Functor, true, DerivedClassesT, DynamicCastObjectCheck> f(object, op);
    }
};

GENERATE_IMPL_TEMPLATE()

void variadic_unittest::test_item_qobject()
{
    SGIHostItemQt hostItem(new QObject);

    typedef generateSGIItemT<SGIItemQt, generateItemAcceptImpl> generateSGIItemFunctor;
    generateSGIItemFunctor func(&hostItem);
    call_function_for_object_type<SGIHostItemQt::ObjectType>(hostItem.object(), func);

    QVERIFY(func.wasAccepted());
    SGIItemBasePtr item = func.getItem();
    QVERIFY(item.valid());

    QCOMPARE(item->score(), 0u);
}

void variadic_unittest::test_item_qmenu()
{
    SGIHostItemQt hostItem(new QMenu);

    typedef generateSGIItemT<SGIItemQt, generateItemAcceptImpl> generateSGIItemFunctor;
    generateSGIItemFunctor func(&hostItem);
    call_function_for_object_type<SGIHostItemQt::ObjectType>(hostItem.object(), func);

    QVERIFY(func.wasAccepted());
    SGIItemBasePtr item = func.getItem();
    QVERIFY(item.valid());

    QCOMPARE(item->score(), 2u);
}

QTEST_MAIN(variadic_unittest)
