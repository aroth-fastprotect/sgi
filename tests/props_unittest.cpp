#include "props_unittest.h"

#include <sstream>
#include <iostream>
#include <typeindex>
#include <type_traits>
#include <functional>

using namespace std;

inline QDebug operator<< (QDebug dbg, const std::string & s)
{
    return dbg << QString::fromStdString(s);
}

class Base
{
protected:
    Base(const std::string & n) : _name(n) {}
public:
    Base() : _name(__FUNCTION__) {}

    const std::string & name() const { return _name; }

protected:
    std::string _name;
};

class A : public Base
{
public:
    A() : Base(__FUNCTION__), _number(42), _fraction(3.1416f) {}

    const int & number() const { return _number; }
    const float & fraction() const { return _fraction; }

protected:
    int _number;
    float _fraction;
};

class B : public Base
{
public:
    B() : Base(__FUNCTION__), _date("1970-01-01") {}

    const std::string & date() const { return _date; }

protected:
    std::string _date;
};

void props_unittest::initTestCase()
{
}

void props_unittest::cleanupTestCase()
{
}

#define PROP_GET(obj, name) \
    std::cout << (obj).get##name() << std::endl
#define PROP(obj, name) \
    std::cout << (obj).name() << std::endl

void print(Base & obj)
{
    PROP(obj,name);
}

void print_A(Base & obj)
{
    PROP(static_cast<A&>(obj),number);
    PROP(static_cast<A&>(obj),fraction);
}

void print_B(Base & obj)
{
    PROP(static_cast<B&>(obj),date);
}

void run(Base & b)
{
    std::map<std::type_index, std::function<void(Base&)> > map;
    map[typeid(B)] = print_B;
    map[typeid(A)] = print_A;

    map[typeid(b)](b);
}

struct myvariant {
    enum Type { Invalid, String, Int, Float } _type;
    std::string _s;
    int _i;
    float _f;
    myvariant() : _type(Invalid), _s(), _i(), _f() {}
    myvariant(const std::string & s) : _type(String), _s(s) {}
    myvariant(const int & s) : _type(Int), _i(s) {}

    template <typename T>
    static myvariant fromValue(const T & f)
    {
        return myvariant(f());
    }
};

template<typename T>
std::string variantToString(const T & t)
{
    return std::to_string(t);
}


void outValue(const std::string & (*f)())
{
    std::cout << f() << std::endl;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const myvariant & v)
{
    switch(v._type)
    {
    case myvariant::String: os << v._s; break;
    case myvariant::Int: os << v._i; break;
    case myvariant::Float: os << v._f; break;
    }
    return os;
}

const std::string & help()
{
    static std::string s("hiha");
    return s;
}

class prop_base {
public:
    prop_base() {}
    virtual void run() {}
};

template<typename T>
class  prop_entry : public prop_base
{
    T & _f;
public:
    prop_entry(T && f) : _f(f) {}
    void run() override {
        _f();
    }
};

typedef std::vector<prop_base*> prop_base_list;

struct TypeDescriptor {
    const char* name;
    size_t size;

    TypeDescriptor(const char* name, size_t size) : name{name}, size{size} {}
    virtual ~TypeDescriptor() {}
    virtual std::string getFullName() const { return name; }
    //virtual void dump(const void* obj, int indentLevel = 0) const = 0;
};

template<typename T, typename RETTYPE>
struct call {
    typedef std::_Mem_fn<RETTYPE T::*> _fn;
    RETTYPE operator()()
    {
        return _fn();
    }
};

template<typename T>
struct getter_type {
    typedef const std::string (T::* getter_str)() const;
};

struct TypeDescriptor_Struct : TypeDescriptor {

    struct Member {
        Member(const char* n, size_t o, TypeDescriptor* t)
            : name(n), offset(o), type(t) {}
        const char* name;
        size_t offset;
        TypeDescriptor* type;
        virtual myvariant get(void * p) const = 0;
    };
    template<typename M, typename T>
    struct MemberT : public Member
    {
        MemberT(const char* n, size_t o, TypeDescriptor* t, M T::* f)
            : Member(n, o, t), func(f) {}
        typedef T type;
        M T::* func;
        myvariant get(void * p) const override {
            T * o = reinterpret_cast<T*>(p);
            auto x = std::bind(func, o);
            return x();
        }
    };

    const Member** members;
    size_t numMembers;
    TypeDescriptor_Struct(const char* name, size_t size, const Member** init, size_t num)
        : TypeDescriptor{name, size}, members{init}, numMembers{num} {
    }
};

template<typename T>
struct registry {
};

#define REG_BEGIN(__class) \
    template<> struct registry<__class> { \
        static TypeDescriptor_Struct Reflection; \
        registry<__class>() : Reflection( { \
    using T = type; \
    typeDesc->name = #__class; \
    typeDesc->size = sizeof(T); \
    typeDesc->members = {

#define REG_END() \
        } ) {} \
    };

#define REG_MEMBER(__m)

// Declare the function template that handles primitive types such as int, std::string, etc.:
template <typename T>
TypeDescriptor* getPrimitiveDescriptor()
{
    return nullptr;
}

// A helper class to find TypeDescriptors in different ways:
struct DefaultResolver {
    template <typename T> static char func(decltype(&T::Reflection));
    template <typename T> static int func(...);
    template <typename T>
    struct IsReflected {
        enum { value = (sizeof(func<T>(nullptr)) == sizeof(char)) };
    };

    // This version is called if T has a static member named "Reflection":
    template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
    static TypeDescriptor* get() {
        return &T::Reflection;
    }

    // This version is called otherwise:
    template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
    static TypeDescriptor* get() {
        return getPrimitiveDescriptor<T>();
    }
};


// This is the primary class template for finding all TypeDescriptors:
template <typename T>
struct TypeResolver {
    static TypeDescriptor* get() {
        return DefaultResolver::get<T>();
    }
};

static const TypeDescriptor_Struct::MemberT a_members_name("name", 0, TypeResolver<decltype(&A::name)>::get(), &A::name);
static const TypeDescriptor_Struct::Member * a_members[] = {
    &a_members_name,
    };


template<> struct registry<A> {
    TypeDescriptor_Struct Reflection;
    registry<A>()
        : Reflection("A", sizeof(A), a_members, sizeof(a_members)/sizeof(a_members[0]))
    {
    }
};


//REG_BEGIN(A)
//REG_MEMBER(name)
//REG_END()

template<typename T>
//struct use_property_getter_helper : std::false_type {};
struct use_property_getter_helper : std::true_type {};

template<typename T>
struct use_property_getter : use_property_getter_helper<typename std::remove_cv<T>::type> {};

template<typename T, bool>
class get_property_impl {
public:
    static myvariant get(const T & t)
    {
        return myvariant();
    }
};

template<typename T>
class get_property_impl<T, false> {
public:
    static myvariant get(const T & t)
    {
        return myvariant(t);
    }
};

template<typename T>
class get_property_impl<T, true> {
public:
    static myvariant get(const T & t)
    {
        return myvariant(t());
    }
};

template<typename T>
inline myvariant get_property(const T & t)
{
    return get_property_impl<T, use_property_getter<T>::value>::get(t);
}




void props_unittest::registerProps()
{
    A a;

    registry<A> aa;
    for(size_t i = 0; i < aa.Reflection.numMembers; ++i)
    {
        const auto & m = *aa.Reflection.members[i];
        //auto x = std::bind(&a, m);
        //decltype(x);
    }

    prop_base_list props;
    //props.push_back(new prop_entry(&A::name));


    typedef std::function<myvariant(void)> obj_func;
    std::vector<obj_func> list_of_getters;

    for(size_t i = 0; i < aa.Reflection.numMembers; ++i)
    {
        const auto & m = *aa.Reflection.members[i];
        std::cout << "m: " << m.name << "=" << m.get(&a) << std::endl;
        //auto x = std::bind(&a, m.func);
        //decltype(x);
    }

    auto nnn = std::bind(&A::name, &a);
    auto v = get_property(nnn);
    std::cout << v << endl;
    //list_of_getters.push_back();

#if 0

    std::cout << nnn() << std::endl;
    std::cout << typeid(nnn).name() << std::endl;

    std::cout << variant::fromValue(nnn) << std::endl;
    //auto fxx = std::bind(&variant::fromValue, std::bind(&A::name, &a));
    //auto fxx = std::bind(&outValue, nnn);
    //variant v = fxx();
    //fxx();
    //list_of_getters.push_back();
    //list_of_getters.push_back(std::bind(&A::number, &a));
    //list_of_getters.push_back(std::bind(&A::fraction, &a));
#endif

    for(auto it : list_of_getters)
    {
        std::cout << it() << std::endl;
    }
}

QTEST_MAIN(props_unittest)
