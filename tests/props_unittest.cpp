#include "props_unittest.h"

#include <sstream>
#include <iostream>
#include <typeindex>


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

struct variant {
    enum Type { String, Int, Float } _type;
    std::string _s;
    int _i;
    float _f;
    variant(const std::string & s) : _type(String), _s(s) {}

    template <typename T>
    static variant fromValue(const T & f)
    {
        return variant(f());
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

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const variant & v)
{
    switch(v._type)
    {
    case variant::String: os << v._s; break;
    case variant::Int: os << v._i; break;
    case variant::Float: os << v._f; break;
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
        const char* name;
        size_t offset;
        TypeDescriptor* type;
        void * func;
    };
    template<typename T>
    struct MemberT : public Member
    {
        typedef T type;
    };

    std::vector<Member> members;
    TypeDescriptor_Struct(const char* name, size_t size, const std::initializer_list<Member>& init)
        : TypeDescriptor{name, size}, members{init} {
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


template<> struct registry<A> {
    TypeDescriptor_Struct Reflection;
    registry<A>()
        : Reflection("A", sizeof(A),
                    {
        { "name", 0, TypeResolver<decltype(&A::name)>::get(), reinterpret_cast<void*>(&A::name)},
}
                     )
    {
    }
};


//REG_BEGIN(A)
//REG_MEMBER(name)
//REG_END()

void props_unittest::registerProps()
{
    A a;

    registry<A> aa;
    for(auto & m : aa.Reflection.members)
    {
        auto x = std::bind(&a, m);
        //decltype(x);
    }

    prop_base_list props;
    //props.push_back(new prop_entry(&A::name));


    typedef std::function<variant(void)> obj_func;
    std::vector<obj_func> list_of_getters;

#if 0
    auto nnn = std::bind(&A::name, &a);
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
