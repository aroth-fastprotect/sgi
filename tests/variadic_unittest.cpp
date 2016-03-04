#include "variadic_unittest.h"
#include "variadic_unittest.moc"

#include <QDebug>
#include <QDialog>
#include <QMenu>
#include <QThread>

#include <sgi/helpers/type_list>

#define SGI_NO_HOSTITEM_GENERATOR
#include <sgi/GenerateItem>
#include <sgi/plugins/SGIHostItemBase.h>
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/plugins/SGIPluginImpl.h>

//#include <sgi/helpers/meta.hpp>

#include <tuple>

using namespace std;
using namespace sgi;
using namespace sgi::helpers;

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
    typedef type_list<QWindow, QWidget, QThread> ddd;

    int n;
    n = index_of<QThread, ddd >::value;
    QCOMPARE(n, 2);

    n = index_of<int, ddd >::value;
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
    typedef type_list<QWindow, QWidget, QThread> ddd;

    counter cnt;
    for_each_type<ddd> aa(cnt);
    QCOMPARE(cnt.num, 3u);

}

void variadic_unittest::test_sizeof()
{
    typedef type_list<QWindow, QWidget, QThread> ddd;

    size_t s = type_list_size<ddd>::value;
    QCOMPARE(s, (size_t)3);

    //QCOMPARE(ddd::size, (size_t)3);

    //QCOMPARE(ddd::size, s);
}

namespace testing {

    template<class T> struct DerivedClassesT : public sgi::details::DerivedClassesImplT<T, sgi::helpers::type_list<> > {}; \
    template<typename BaseType, typename Functor> \
    struct call_function_for_object_type { \
        call_function_for_object_type(BaseType * object, Functor & op) { \
            details::call_function_for_object_type<BaseType, Functor, DerivedClassesT>(object, op); \
        } \
    };


    //template<> struct DerivedClassesT<__base_type> : public details::DerivedClassesImplT<__base_type, __derived_types> {};
}


struct QObjectCaster
{
    template <class To, class From>
    static To* cast(From* obj)
    {
        return qobject_cast<To*>(obj);
    }
    template <class To, class From>
    static const To* cast(const From* obj)
    {
        return qobject_cast<const To*>(obj);
    }
};

namespace details {
    template<typename BASE>
    struct DerivedClassImplT {
        typedef DERIVED derived_types;
    };

    template<typename BASE, typename DERIVED = type_list<>, typename OBJECT_CASTER = DynamicCaster >
    struct BaseClassImplT {
        typedef BASE base_type;
        typedef DERIVED derived_types;
        typedef OBJECT_CASTER object_caster;
    };
}

template<typename BASE>
struct BaseClass : public details::BaseClassImplT<BASE>
{
};

template<typename BASE, typename DERIVED=type_list<>, typename OBJECT_CASTER=DynamicCaster >
struct ClassTypeInfo {
    typedef BASE base_type;
    typedef DERIVED derived_types;
    typedef OBJECT_CASTER object_caster;
};

template<>
struct ClassTypeInfo<QObject, type_list<QWindow, QWidget, QThread>, QObjectCaster > {};

template<>
struct ClassTypeInfo<QWidget, type_list<QMenu, QDialog>, QObjectCaster > {};

#if 0
template <typename BaseType, typename Functor, bool startOfLevel, template<typename> class DerivedClassesT, template<typename, typename> class ObjectTypeCheckOperatorT>
struct call_function_for_object_and_derivedT<BaseType, sgi::helpers::type_list<>, Functor, startOfLevel, DerivedClassesT, ObjectTypeCheckOperatorT>
{
    call_function_for_object_and_derivedT(BaseType * object, Functor & op)
    {
        if (op.recursiveAccept())
        {
            if (startOfLevel && op.canAccept(object))
                op.accept(object);
        }
        else
        {
            if (!op.wasAccepted() && op.canAccept(object))
                op.accept(object);
        }
    }
};

template <typename BaseType, class Head, class ...Tail, typename Functor, bool startOfLevel, template<typename> class DerivedClassesT, template<typename, typename> class ObjectTypeCheckOperatorT>
struct call_function_for_object_and_derivedT<BaseType, sgi::helpers::type_list<Head, Tail...>, Functor, startOfLevel, DerivedClassesT, ObjectTypeCheckOperatorT>
{
    call_function_for_object_and_derivedT(BaseType * object, Functor & op)
    {
        if (startOfLevel && op.canAccept(object) && op.recursiveAccept())
            op.accept(object);

        if (Head * derivedTypeObject = ObjectTypeCheckOperatorT<Head, BaseType>::getObject(object)) {
            op.decend(derivedTypeObject);
            call_function_for_object_and_derivedT<Head, typename DerivedClassesT<Head>::DerivedClasses, Functor, true, DerivedClassesT, ObjectTypeCheckOperatorT> f(derivedTypeObject, op);
            op.ascend(derivedTypeObject);
            if (!op.wasAccepted() && op.canAccept(derivedTypeObject) && !op.recursiveAccept()) {
                op.accept(derivedTypeObject);
            }
        }
        else
        {
            call_function_for_object_and_derivedT<BaseType, sgi::helpers::type_list<Tail...>, Functor, false, DerivedClassesT, DynamicCastObjectCheck> tail(object, op);
            if (!op.wasAccepted() && op.canAccept(object) && !op.recursiveAccept()) {
                op.accept(object);
            }
        }
    }
};
template<typename BaseType, typename Functor, template<typename> class DerivedClassesT>
struct call_function_for_object_type {
    call_function_for_object_type(BaseType * object, Functor & op) {
        call_function_for_object_and_derivedT<BaseType, typename DerivedClassesT<BaseType>::DerivedClasses, Functor, true, DerivedClassesT, DynamicCastObjectCheck> f(object, op);
    }
};
#endif

template<typename BaseType>
struct call_function_for_object_type {
    template<typename Functor>
    call_function_for_object_type(BaseType * object, Functor & op) {
        typedef ClassTypeInfo<BaseType>::derived_types derived_types;
        counter cnt;
        for_each_type<derived_types> aa(cnt);
        //call_function_for_object_and_derivedT<BaseType, typename DerivedClassesT<BaseType>::DerivedClasses, Functor, true, DerivedClassesT, DynamicCastObjectCheck> f(object, op);
    }
};

void variadic_unittest::test_item_generation()
{
    QMenu * menu = new QMenu;
    SGIHostItemQt hostItem(menu);
    SGIItemBasePtr item;

    details::objectTypeTraverseOutput func(std::cout);

    call_function_for_object_type<QObject>(menu, func);
    //generateItem(item, &hostItem);

}

QTEST_MAIN(variadic_unittest)
