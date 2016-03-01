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

//The typelist template
template<typename... Ts>
struct type_list{};

//A metafunction returning the index of a type T in a type_list of types L. If T doesn't belong to L, it returns -1
template<typename T , typename L>
struct index_of;

//Again that recursive head:tail traversal of functional languages
template<typename T , typename HEAD , typename... TAIL>
struct index_of<T,type_list<HEAD,TAIL...>>
{
 static constexpr int next_value = index_of<T, type_list<TAIL...> >::value;
 static constexpr int value = next_value >= 0 ? next_value + 1 : -1; //Propagate the -1 or the index
};

//First base case: The type is found (T matches HEAD)
template<typename T , typename... TAIL>
struct index_of<T,type_list<T,TAIL...>>
{
 static constexpr int vaue = 0;
};

//Second base case: The type is not found (There are no more types on the type_list to match with)
template<typename T>
struct index_of<T,type_list<>>
{
 static constexpr int value = -1;
};

template<std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type
  _for_each(std::tuple<Tp...> &, FuncT) // Unused arguments are given no names.
  { }

template<std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if<I < sizeof...(Tp), void>::type
  _for_each(std::tuple<Tp...>& t, FuncT f)
  {
    f(std::get<I>(t));
    for_each<I + 1, FuncT, Tp...>(t, f);
  }

struct print
{
    template<typename T>
    void operator()(T t)
    {
        std::cout << typeid(T).name();
    }
};

template<typename TL>
void dump_all_types()
{
  //  _for_each( TL(), print());
}

void variadic_unittest::checkCompiler()
{
    typedef type_list<QWindow, QWidget, QThread>  ddd;

    SGIHostItemQt item_app(QCoreApplication::instance());

    int n = index_of<QThread, ddd>();

    dump_all_types<ddd>();


}

QTEST_MAIN(variadic_unittest)
