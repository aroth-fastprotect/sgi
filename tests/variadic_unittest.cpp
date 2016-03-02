#include "variadic_unittest.h"
#include "variadic_unittest.moc"

#include <QDebug>
#include <QDialog>
#include <QMenu>
#include <QThread>

#define SGI_NO_HOSTITEM_GENERATOR
#include <sgi/GenerateItem>
#include <sgi/plugins/SGIHostItemBase.h>
#include <sgi/plugins/SGIHostItemQt.h>
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

//A metafunction returning the index of a type T in a type_list of types L. If T doesn't belong to L, it returns -1
template<typename T>
struct type_list_size;

//The typelist template
template<typename... TYPES>
struct type_list
{
    static constexpr size_t size = type_list_size<type_list<TYPES...> >::value;
};


//Again that recursive head:tail traversal of functional languages
template<typename HEAD, typename... TAIL>
struct type_list_size<type_list<HEAD,TAIL...>>
{
    static constexpr size_t value = type_list_size<type_list<TAIL...> >::value + 1u;
};

//Second base case: The type is not found (There are no more types on the type_list to match with)
template<>
struct type_list_size<type_list<> >
{
    static constexpr size_t value = 0;
};

//A metafunction returning the index of a type T in a type_list of types L. If T doesn't belong to L, it returns -1
template<typename T , typename L>
struct index_of;

//Again that recursive head:tail traversal of functional languages
template<typename T , typename HEAD , typename... TAIL>
struct index_of<T,type_list<HEAD,TAIL...>>
{
    static constexpr int next_value = index_of<T,type_list<TAIL...> >::value;
    static constexpr int value = next_value >= 0 ? next_value + 1 : -1; //Propagate the -1 or the index
};

//First base case: The type is found (T matches HEAD)
template<typename T , typename... TAIL>
struct index_of<T,type_list<T,TAIL...>>
{
    static constexpr int value = 0;
};

//Second base case: The type is not found (There are no more types on the type_list to match with)
template<typename T>
struct index_of<T,type_list<>>
{
    static constexpr int value = -1;
};

void variadic_unittest::test_index_of()
{
    typedef type_list<QWindow, QWidget, QThread> ddd;

    int n;
    n = index_of<QThread, ddd >::value;
    QCOMPARE(n, 2);

    n = index_of<int, ddd >::value;
    QCOMPARE(n, -1);
}

//A metafunction returning the index of a type T in a type_list of types L. If T doesn't belong to L, it returns -1
template<typename T>
struct for_each_type;

//Again that recursive head:tail traversal of functional languages
template<typename HEAD, typename... TAIL>
struct for_each_type<type_list<HEAD,TAIL...>>
{
    template<typename FuncT>
    for_each_type(FuncT & f)
    {
        f.template operator()<HEAD>();
        //std::cout << typeid(HEAD).name();
        //f<typename HEAD>();
        for_each_type<type_list<TAIL...> > next(f);
    }
};

//Second base case: The type is not found (There are no more types on the type_list to match with)
template<>
struct for_each_type<type_list<> >
{
    template<typename FuncT>
    for_each_type(FuncT &)
    {
    }
};

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


QTEST_MAIN(variadic_unittest)
