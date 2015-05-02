// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#pragma once

#include "SGIItemBase.h"
#include "SGIPluginInterface.h"
#include <typeindex>
#include <unordered_map>
#include <string.h>

#include "../helpers/NullType"

namespace sgi {

class SGIItemBase;
class SGIPluginHostInterface;

namespace details {
    template<bool value_>
    struct constexpr_bool {
#ifdef __GNUG__
        constexpr static const bool value = value_;
#else
        static const bool value = value_;
#endif
    };
    typedef constexpr_bool<true> constexpr_true;
    typedef constexpr_bool<false> constexpr_false;

    template<typename EnumType, EnumType value_>
    struct constexpr_enum {
#ifdef __GNUG__
        constexpr static const EnumType value = value_;
#else
        static const EnumType value = value_;
#endif
    };

    template<typename CallParamType, typename ResultType>
    class call_by_type_interface
    {
    public:
        virtual bool callImpl(const CallParamType & param, ResultType & result) = 0;
    };

    template<typename TypePolicy>
    class call_by_type_registryT
    {
    protected:
        call_by_type_registryT() {}
    public:
        typedef typename TypePolicy::CallParamType CallParamType;
        typedef typename TypePolicy::ResultType ResultType;
        typedef call_by_type_interface<CallParamType, ResultType> InterfaceType;
    public:
        bool call(const std::type_info & ti, const CallParamType & param, ResultType & result)
        {
            bool ret;
            InterfaceType * typeImpl = _registed_types[std::type_index(ti)];
            ret = (typeImpl != NULL);
            if(ret)
                ret = typeImpl->callImpl(param, result);
            return ret;
        }

        void registerType(const std::type_index & ti, InterfaceType* impl)
        {
            _registed_types[ti] = impl;
        }

        static call_by_type_registryT * instance()
        {
            static call_by_type_registryT s_instance;
            return &s_instance;
        }

    private:
        typedef std::unordered_map<std::type_index, InterfaceType*> TypeMap;
        TypeMap _registed_types;
    };

    template<typename T>
    struct TypeWrap {
        typedef T Type;
    };

    template<unsigned num>
    struct UintWrap {
        typedef unsigned Type;
#ifdef __GNUG__
        constexpr static const unsigned value = num;
#else
        static const unsigned value = num;
#endif
    };

    template<typename CallImplT, typename T>
    class call_by_type_auto_registerT
    {
    public:
        typedef typename CallImplT::Registry RegistryType;
        typedef typename RegistryType::CallParamType CallParamType;
        typedef typename RegistryType::ResultType ResultType;

        typedef call_by_type_interface<CallParamType, ResultType> InterfaceType;

        class InterfaceWrapper : public InterfaceType
        {
        public:
            virtual bool callImpl(const CallParamType & param, ResultType & result)
            {
                return CallImplT::callImpl(TypeWrap<T>(), param, result);
            }
        };
        call_by_type_auto_registerT()
        {
            RegistryType::instance()->registerType(typeid(T), &_iface);
        }
    private:
        InterfaceWrapper _iface;
    };

    template<typename IdPolicy>
    class call_by_id_registryT
    {
    protected:
        call_by_id_registryT() {}
    public:
        typedef typename IdPolicy::CallParamType CallParamType;
        typedef typename IdPolicy::ResultType ResultType;
        typedef call_by_type_interface<CallParamType, ResultType> InterfaceType;
    public:
        bool call(const unsigned id, const CallParamType & param, ResultType & result)
        {
            bool ret;
            InterfaceType * typeImpl = _registed_types[id];
            ret = (typeImpl != NULL);
            if(ret)
                ret = typeImpl->callImpl(param, result);
            return ret;
        }

        void registerId(const unsigned id, InterfaceType* impl)
        {
            _registed_types[id] = impl;
        }

        static call_by_id_registryT * instance()
        {
            static call_by_id_registryT s_instance;
            return &s_instance;
        }

    private:
        typedef std::unordered_map<unsigned, InterfaceType*> TypeMap;
        TypeMap _registed_types;
    };

    template<typename CallImplT, unsigned Id>
    class call_by_id_auto_registerT
    {
    public:
        typedef typename CallImplT::Registry RegistryType;
        typedef typename RegistryType::CallParamType CallParamType;
        typedef typename RegistryType::ResultType ResultType;

        typedef call_by_type_interface<CallParamType, ResultType> InterfaceType;

        class InterfaceWrapper : public InterfaceType
        {
        public:
            virtual bool callImpl(const CallParamType & param, ResultType & result)
            {
                return CallImplT::callImpl(UintWrap<Id>(), param, result);
            }
        };
        call_by_id_auto_registerT()
        {
            RegistryType::instance()->registerId(Id, &_iface);
        }
    private:
        InterfaceWrapper _iface;
    };

    template<typename TYPENAME_OF>
    class StaticTypeNameImpl
    {
    public:
        StaticTypeNameImpl()
        {
#ifdef _MSC_VER
            const char * begin = __FUNCTION__;
            const size_t len = strlen(begin);
#else
            const char * begin = strstr(__PRETTY_FUNCTION__, "TYPENAME_OF = ");
            begin += 14;
            const size_t len = strlen(begin) - 1;
#endif
            name.assign(begin, len);
        }
        std::string name;
    };

    template<typename T>
    struct StaticTypeName
    {
        static const std::string & name()
        {
            static std::string name_of_this_enum(StaticTypeNameImpl<T>().name);
            return name_of_this_enum;
        }
    };

} // namespace details

class functionImplBase
{
public:
    functionImplBase(SGIPluginHostInterface * hostInterface, SGIItemBase * item=NULL)
        : _hostInterface(hostInterface)
        , _item(item)
        {
        }
    functionImplBase(SGIPluginHostInterface * hostInterface, const SGIItemBase * item=NULL)
        : _hostInterface(hostInterface)
        , _item(const_cast<SGIItemBase*>(item))
        {
        }

    template<typename ANOTHER_ITEMTYPE>
    ANOTHER_ITEMTYPE * item()
    {
        return static_cast<ANOTHER_ITEMTYPE *>(_item.get());
    }
    template<typename ANOTHER_ITEMTYPE>
    const ANOTHER_ITEMTYPE * item() const
    {
        return static_cast<const ANOTHER_ITEMTYPE *>(_item.get());
    }
    template<typename ANOTHER_ITEMTYPE>
    ANOTHER_ITEMTYPE * itemAs()
    {
        return dynamic_cast<ANOTHER_ITEMTYPE *>(_item.get());
    }
    template<typename ANOTHER_ITEMTYPE>
    const ANOTHER_ITEMTYPE * itemAs() const
    {
        return dynamic_cast<const ANOTHER_ITEMTYPE *>(_item.get());
    }

    template<typename ANOTHER_ITEMTYPE, typename SGIITEMTYPE_ENUM>
    ANOTHER_ITEMTYPE * cloneItem(SGIITEMTYPE_ENUM newType=(SGIITEMTYPE_ENUM)0, const osg::CopyOp & copyop=osg::CopyOp::SHALLOW_COPY)
    {
        return _item->rootBase()->clone<ANOTHER_ITEMTYPE>((SGIItemType)newType, copyop);
    }
    template<typename ANOTHER_ITEMTYPE, typename SGIITEMTYPE_ENUM>
    ANOTHER_ITEMTYPE * cloneItem(SGIITEMTYPE_ENUM newType, osg::Referenced * userData, const osg::CopyOp & copyop=osg::CopyOp::SHALLOW_COPY)
    {
        return _item->rootBase()->clone<ANOTHER_ITEMTYPE>((SGIItemType)newType, userData, copyop);
    }
    template<typename ANOTHER_ITEMTYPE, typename SGIITEMTYPE_ENUM>
    ANOTHER_ITEMTYPE * cloneItem(SGIITEMTYPE_ENUM newType, unsigned number, const osg::CopyOp & copyop=osg::CopyOp::SHALLOW_COPY)
    {
        return _item->rootBase()->clone<ANOTHER_ITEMTYPE>((SGIItemType)newType, number, copyop);
    }
    template<typename ANOTHER_ITEMTYPE, typename SGIITEMTYPE_ENUM>
    ANOTHER_ITEMTYPE * cloneItem(SGIITEMTYPE_ENUM newType, unsigned number, osg::Referenced * userData, const osg::CopyOp & copyop=osg::CopyOp::SHALLOW_COPY)
    {
        return _item->rootBase()->clone<ANOTHER_ITEMTYPE>((SGIItemType)newType, number, userData, copyop);
    }

    template<typename ItemTypeFirst, typename ItemTypeSecond, typename SGIITEMTYPE_ENUM>
    SGIItemBase * cloneItemMulti(SGIITEMTYPE_ENUM newType=(SGIITEMTYPE_ENUM)0, const osg::CopyOp & copyop=osg::CopyOp::SHALLOW_COPY)
    {
        SGIItemBase * ret = NULL;
        if(ItemTypeFirst * first = dynamic_cast<ItemTypeFirst*>(_item.get()))
            ret = _item->rootBase()->clone<ItemTypeFirst>((SGIItemType)newType, copyop);
        else if(ItemTypeSecond * second = dynamic_cast<ItemTypeSecond*>(_item.get()))
            ret = _item->rootBase()->clone<ItemTypeSecond>((SGIItemType)newType, copyop);
        return ret;
    }
    template<typename T, typename ItemType>
    inline T * getObject()
    {
        return getObject<T, ItemType, StaticCaster>();
    }
    template<typename T, typename ItemType >
    inline const T * getObject() const
    {
        return getObject<T, ItemType, StaticCaster>();
    }
    template<typename T, typename ItemType, template<typename, typename> class CasterT>
    T * getObject()
    {
        typedef typename ItemType::ObjectType ObjectType;
        ObjectType * obj = static_cast<const ItemType*>(_item.get())->object();
        T * ret = CasterT<T, ObjectType>::cast(obj);
        return ret;
    }
    template<typename T, typename ItemType, template<typename, typename> class CasterT>
    const T * getObject() const
    {
        typedef typename ItemType::ObjectType ObjectType;
        const ObjectType * obj = static_cast<const ItemType*>(_item.get())->object();
        const T * ret = CasterT<const T, ObjectType>::cast(obj);
        return ret;
    }
    template<typename T, typename ItemTypeFirst, typename ItemTypeSecond>
    inline T * getObjectMulti()
    {
        return getObjectMulti<T, ItemTypeFirst, ItemTypeSecond, StaticCaster>();
    }
    template<typename T, typename ItemTypeFirst, typename ItemTypeSecond>
    inline const T * getObjectMulti() const
    {
        return getObjectMulti<T, ItemTypeFirst, ItemTypeSecond, StaticCaster>();
    }
    template<typename T, typename ItemTypeFirst, typename ItemTypeSecond, template<typename, typename> class CasterT>
    T * getObjectMulti()
    {
        T * ret = NULL;
        if(ItemTypeFirst * first = dynamic_cast<ItemTypeFirst*>(_item.get()))
        {
            typedef typename ItemTypeFirst::ObjectType ObjectType;
            ObjectType * obj = first->object();
            ret = CasterT<T, ObjectType>::cast(obj);
        }
        else if(ItemTypeSecond * second = dynamic_cast<ItemTypeSecond*>(_item.get()))
        {
            typedef typename ItemTypeSecond::ObjectType ObjectType;
            ObjectType * obj = second->object();
            ret = CasterT<T, ObjectType>::cast(obj);
        }
        return ret;
    }
    template<typename T, typename ItemTypeFirst, typename ItemTypeSecond, template<typename, typename> class CasterT>
    const T * getObjectMulti() const
    {
        const T * ret = NULL;
        if(const ItemTypeFirst * first = dynamic_cast<const ItemTypeFirst*>(_item.get()))
        {
            typedef typename ItemTypeFirst::ObjectType ObjectType;
            const ObjectType * obj = first->object();
            ret = CasterT<const T, ObjectType>::cast(obj);
        }
        else if(const ItemTypeSecond * second = dynamic_cast<const ItemTypeSecond*>(_item.get()))
        {
            typedef typename ItemTypeSecond::ObjectType ObjectType;
            const ObjectType * obj = second->object();
            ret = CasterT<const T, ObjectType>::cast(obj);
        }
        return ret;
    }

    template<typename T>
    std::string enumValueToString(T value)
    {
        std::string ret;
        const std::string& value_type = details::StaticTypeName<T>::name();
        _hostInterface->namedEnumValueToString(value_type, ret, (int)value);
        return ret;
    }

    template<typename T, typename INPUT_TYPE>
    std::string castToEnumValueString(INPUT_TYPE value)
    {
        std::string ret;
        const std::string& value_type = details::StaticTypeName<T>::name();
        _hostInterface->namedEnumValueToString(value_type, ret, (int)value);
        return ret;
    }

    SGIItemType itemType() const
    {
        return _item->type();
    }
    unsigned itemNumber() const
    {
        return _item->number();
    }

protected:
    SGIPluginHostInterface * _hostInterface;
    SGIItemBasePtr _item;
};

template<typename T>
std::string enumValueToString(T value)
{
    std::string ret;
    const std::string& value_type = details::StaticTypeName<T>::name();
    SGIPluginInterface::hostInterface()->namedEnumValueToString(value_type, ret, (int)value);
    return ret;
}

template<typename T, typename INPUT_TYPE>
std::string castToEnumValueString(INPUT_TYPE value)
{
    std::string ret;
    const std::string& value_type = details::StaticTypeName<T>::name();
    SGIPluginInterface::hostInterface()->namedEnumValueToString(value_type, ret, (int)value);
    return ret;
}

template<typename CallParamType, typename ResultType, typename SGIItemType=SGIItemBase>
class functionImplBaseT : public functionImplBase
{
public:
    functionImplBaseT(SGIPluginHostInterface * hostInterface, SGIItemType * item=NULL)
        : functionImplBase(hostInterface, item)
        {
        }

public:
    class Registry
    {
    public:
        static bool call(SGIPluginHostInterface * hostInterface, ResultType & result, CallParamType param, SGIItemType * item)
        {
            bool ret;
            const std::type_info * ti = item->typeInfo();
            Registry * typeImpl = _registed_types[std::type_index(*ti)];
            ret = (typeImpl != NULL);
            if(ret)
                result = typeImpl->callImpl(hostInterface, param, item);
            return ret;
        }

    protected:
        virtual ResultType callImpl(SGIPluginHostInterface * hostInterface, CallParamType param, SGIItemType * item) = 0;

        static void registerType(const std::type_index & ti, Registry * impl)
        {
            _registed_types[ti] = impl;
        }

        typedef std::unordered_map<std::type_index, Registry *> TypeMap;
        static TypeMap _registed_types;
    };

protected:
    template<typename T, template<typename> class ImplT>
    class AutoRegister : public Registry {
    public:
        AutoRegister() {
            Registry::registerType(typeid(T), this);
        }
        virtual ResultType callImpl(SGIPluginHostInterface * hostInterface, CallParamType param, SGIItemType * item) {
            ImplT<T> impl(hostInterface, item);
            ResultType ret = impl.process(param);
            return ret;
        }
    };
};

template<template<typename> class ImplT>
class writePrettyHTMLImplBaseT : public functionImplBase
{
public:
    struct TypePolicy {
        struct CallParamType
        {
            CallParamType(SGIPluginHostInterface * hostInterface_, const SGIItemBase * item_, std::basic_ostream<char>& os_, bool table_)
                : hostInterface(hostInterface_), item(item_), os(os_), table(table_) {}
            SGIPluginHostInterface * hostInterface;
            const SGIItemBase * item;
            std::ostream & os;
            bool table;
        };
        typedef bool ResultType;
    };

    writePrettyHTMLImplBaseT(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL, bool table=true)
        : functionImplBase(hostInterface, item), _table(table) {}

    typedef details::call_by_type_registryT<TypePolicy> Registry;
    template<typename T>
    class AutoRegisterTypeT : public details::call_by_type_auto_registerT<writePrettyHTMLImplBaseT, T>
    {
    };

    typedef typename TypePolicy::CallParamType CallParamType;
    typedef typename TypePolicy::ResultType ResultType;

    template<typename T>
    static bool callImpl(const details::TypeWrap<T> & t, const CallParamType & param, ResultType & result)
    {
        ImplT<T> f(param.hostInterface, param.item, param.table);
        result = f.process(param.os);
        return result;
    }

    static bool call(SGIPluginHostInterface * hostInterface, const SGIItemBase * item, std::basic_ostream<char>& os, bool table)
    {
        CallParamType param(hostInterface, item, os, table);
        ResultType result;
        return Registry::instance()->call(*item->typeInfo(), param, result);
    }

    bool callNextHandler(std::basic_ostream<char>& os)
    {
        bool ret = false;
        SGIItemBase * itemNext = _item->nextBase();
        if(itemNext)
        {
            bool table = (os.tellp() == (std::streamoff)0);
            ret = _hostInterface->writePrettyHTML(os, itemNext, table);
        }
        return ret;
    }

protected:
    bool    _table;
};

#define WRITE_PRETTY_HTML_IMPL_TEMPLATE() \
    template<typename T> \
    class writePrettyHTMLImpl { \
    public: \
        typedef sgi::details::constexpr_false accept; \
        writePrettyHTMLImpl(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL, bool table=true) {} \
        bool process(std::basic_ostream<char>& os) { return false; } \
    }; \
    typedef writePrettyHTMLImplBaseT<writePrettyHTMLImpl> writePrettyHTMLImplBase;

#define WRITE_PRETTY_HTML_IMPL_DECLARE(__type) \
    template<> \
    class writePrettyHTMLImpl<__type> : public writePrettyHTMLImplBase { \
    private: \
        typedef AutoRegisterTypeT<__type> AutoRegisterType; \
        static AutoRegisterType s_autoregister; \
    public: \
        typedef sgi::details::constexpr_true accept; \
        writePrettyHTMLImpl<__type>(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL, bool table=true) \
            : writePrettyHTMLImplBase(hostInterface, item, table) {} \
        bool process(std::basic_ostream<char>& os); \
    };

#define WRITE_PRETTY_HTML_IMPL_REGISTER(__type) \
    writePrettyHTMLImpl<__type>::AutoRegisterType writePrettyHTMLImpl<__type>::s_autoregister;

#define WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(__type) \
    WRITE_PRETTY_HTML_IMPL_DECLARE(__type) \
    WRITE_PRETTY_HTML_IMPL_REGISTER(__type)

template<template<typename> class ImplT>
class getObjectInfoFullStringImplBaseT : public functionImplBase
{
public:
    struct TypePolicy {
        struct CallParamType
        {
            CallParamType(SGIPluginHostInterface * hostInterface_, const SGIItemBase * item_, bool full_)
                : hostInterface(hostInterface_), item(item_), full(full_) {}
            SGIPluginHostInterface * hostInterface;
            const SGIItemBase * item;
            bool full;
        };
        typedef std::string ResultType;
    };

    getObjectInfoFullStringImplBaseT(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL, bool full=true)
        : functionImplBase(hostInterface, item), _full(full) {}

    typedef details::call_by_type_registryT<TypePolicy> Registry;
    template<typename T>
    class AutoRegisterTypeT : public details::call_by_type_auto_registerT<getObjectInfoFullStringImplBaseT, T>
    {
    };

    typedef typename TypePolicy::CallParamType CallParamType;
    typedef typename TypePolicy::ResultType ResultType;

    template<typename T>
    static bool callImpl(const details::TypeWrap<T> & t, const CallParamType & param, ResultType & result)
    {
        ImplT<T> f(param.hostInterface, param.item);
        result = f.process();
        return !result.empty();
    }

    static bool call(SGIPluginHostInterface * hostInterface, const SGIItemBase * item, std::string & result, bool full)
    {
        CallParamType param(hostInterface, item, full);
        return Registry::instance()->call(*item->typeInfo(), param, result);
    }

protected:
    bool    _full;
};

#define GET_OBJECT_NAME_IMPL_TEMPLATE() \
    template<typename T> \
    class getObjectNameImpl { \
    public: \
        typedef sgi::details::constexpr_false accept; \
        getObjectNameImpl(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL, bool full=true) {} \
        std::string process() { return std::string(); } \
    }; \
    typedef getObjectInfoFullStringImplBaseT<getObjectNameImpl> getObjectNameImplBase;

#define GET_OBJECT_NAME_IMPL_DECLARE(__type) \
    template<> \
    class getObjectNameImpl<__type> : public getObjectNameImplBase { \
    private: \
        typedef AutoRegisterTypeT<__type> AutoRegisterType; \
        static AutoRegisterType s_autoregister; \
    public: \
        typedef sgi::details::constexpr_true accept; \
        getObjectNameImpl<__type>(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL, bool full=true) \
            : getObjectNameImplBase(hostInterface, item, full) {} \
        std::string process(); \
    };

#define GET_OBJECT_NAME_IMPL_REGISTER(__type) \
    getObjectNameImpl<__type>::AutoRegisterType getObjectNameImpl<__type>::s_autoregister;

#define GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(__type) \
    GET_OBJECT_NAME_IMPL_DECLARE(__type) \
    GET_OBJECT_NAME_IMPL_REGISTER(__type)

#define GET_OBJECT_DISPLAYNAME_IMPL_TEMPLATE() \
    template<typename T> \
    class getObjectDisplayNameImpl { \
    public: \
        typedef sgi::details::constexpr_false accept; \
        getObjectDisplayNameImpl(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL, bool full=true) {} \
        std::string process() { return std::string(); } \
    }; \
    typedef getObjectInfoFullStringImplBaseT<getObjectDisplayNameImpl> getObjectDisplayNameImplBase;

#define GET_OBJECT_DISPLAYNAME_IMPL_DECLARE(__type) \
    template<> \
    class getObjectDisplayNameImpl<__type> : public getObjectDisplayNameImplBase { \
    private: \
        typedef AutoRegisterTypeT<__type> AutoRegisterType; \
        static AutoRegisterType s_autoregister; \
    public: \
        typedef sgi::details::constexpr_true accept; \
        getObjectDisplayNameImpl<__type>(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL, bool full=true) \
            : getObjectDisplayNameImplBase(hostInterface, item, full) {} \
        std::string process(); \
    };

#define GET_OBJECT_DISPLAYNAME_IMPL_REGISTER(__type) \
    getObjectDisplayNameImpl<__type>::AutoRegisterType getObjectDisplayNameImpl<__type>::s_autoregister;

#define GET_OBJECT_DISPLAYNAME_IMPL_DECLARE_AND_REGISTER(__type) \
    GET_OBJECT_DISPLAYNAME_IMPL_DECLARE(__type) \
    GET_OBJECT_DISPLAYNAME_IMPL_REGISTER(__type)

#define GET_OBJECT_TYPE_IMPL_TEMPLATE() \
    template<typename T> \
    class getObjectTypeImpl { \
    public: \
        typedef sgi::details::constexpr_false accept; \
        getObjectTypeImpl(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL, bool full=true) {} \
        std::string process() { return std::string(); } \
    }; \
    typedef getObjectInfoFullStringImplBaseT<getObjectTypeImpl> getObjectTypeImplBase;

#define GET_OBJECT_TYPE_IMPL_DECLARE(__type) \
    template<> \
    class getObjectTypeImpl<__type> : public getObjectTypeImplBase { \
    private: \
        typedef AutoRegisterTypeT<__type> AutoRegisterType; \
        static AutoRegisterType s_autoregister; \
    public: \
        typedef sgi::details::constexpr_true accept; \
        getObjectTypeImpl<__type>(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL, bool full=true) \
            : getObjectTypeImplBase(hostInterface, item, full) {} \
        std::string process(); \
    };

#define GET_OBJECT_TYPE_IMPL_REGISTER(__type) \
    getObjectTypeImpl<__type>::AutoRegisterType getObjectTypeImpl<__type>::s_autoregister;

#define GET_OBJECT_TYPE_IMPL_DECLARE_AND_REGISTER(__type) \
    GET_OBJECT_TYPE_IMPL_DECLARE(__type) \
    GET_OBJECT_TYPE_IMPL_REGISTER(__type)

template<template<typename> class ImplT>
class getObjectInfoStringImplBaseT : public functionImplBase
{
public:
    struct TypePolicy {
        struct CallParamType
        {
            CallParamType(SGIPluginHostInterface * hostInterface_, const SGIItemBase * item_)
                : hostInterface(hostInterface_), item(item_) {}
            SGIPluginHostInterface * hostInterface;
            const SGIItemBase * item;
        };
        typedef std::string ResultType;
    };

    getObjectInfoStringImplBaseT(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL)
        : functionImplBase(hostInterface, item) {}

    typedef details::call_by_type_registryT<TypePolicy> Registry;
    template<typename T>
    class AutoRegisterTypeT : public details::call_by_type_auto_registerT<getObjectInfoStringImplBaseT, T>
    {
    };

    typedef typename TypePolicy::CallParamType CallParamType;
    typedef typename TypePolicy::ResultType ResultType;

    template<typename T>
    static bool callImpl(const details::TypeWrap<T> & t, const CallParamType & param, ResultType & result)
    {
        ImplT<T> f(param.hostInterface, param.item);
        result = f.process();
        return !result.empty();
    }

    static bool call(SGIPluginHostInterface * hostInterface, const SGIItemBase * item, std::string & result)
    {
        CallParamType param(hostInterface, item);
        return Registry::instance()->call(*item->typeInfo(), param, result);
    }
};

#define GET_OBJECT_SUGGESTED_FILENAME_IMPL_TEMPLATE() \
    template<typename T> \
    class getObjectSuggestedFilenameImpl { \
    public: \
        typedef sgi::details::constexpr_false accept; \
        getObjectSuggestedFilenameImpl(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL) {} \
        std::string process() { return std::string(); } \
    }; \
    typedef getObjectInfoStringImplBaseT<getObjectSuggestedFilenameImpl> getObjectSuggestedFilenameImplBase;

#define GET_OBJECT_SUGGESTED_FILENAME_IMPL_DECLARE(__type) \
    template<> \
    class getObjectSuggestedFilenameImpl<__type> : public getObjectSuggestedFilenameImplBase { \
    private: \
        typedef AutoRegisterTypeT<__type> AutoRegisterType; \
        static AutoRegisterType s_autoregister; \
    public: \
        typedef sgi::details::constexpr_true accept; \
        getObjectSuggestedFilenameImpl<__type>(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL) \
            : getObjectSuggestedFilenameImplBase(hostInterface, item) {} \
        std::string process(); \
    };

#define GET_OBJECT_SUGGESTED_FILENAME_IMPL_REGISTER(__type) \
    getObjectSuggestedFilenameImpl<__type>::AutoRegisterType getObjectSuggestedFilenameImpl<__type>::s_autoregister;

#define GET_OBJECT_SUGGESTED_FILENAME_EXTENSION_IMPL_TEMPLATE() \
    template<typename T> \
    class getObjectSuggestedFilenameExtensionImpl { \
    public: \
        typedef sgi::details::constexpr_false accept; \
        getObjectSuggestedFilenameExtensionImpl(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL) {} \
        std::string process() { return std::string(); } \
    }; \
    typedef getObjectInfoStringImplBaseT<getObjectSuggestedFilenameExtensionImpl> getObjectSuggestedFilenameExtensionImplBase;

#define GET_OBJECT_SUGGESTED_FILENAME_EXTENSION_IMPL_DECLARE(__type) \
    template<> \
    class getObjectSuggestedFilenameExtensionImpl<__type> : public getObjectSuggestedFilenameExtensionImplBase { \
    private: \
        typedef AutoRegisterTypeT<__type> AutoRegisterType; \
        static AutoRegisterType s_autoregister; \
    public: \
        typedef sgi::details::constexpr_true accept; \
        getObjectSuggestedFilenameExtensionImpl<__type>(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL) \
            : getObjectSuggestedFilenameExtensionImplBase(hostInterface, item) {} \
        std::string process(); \
    };

#define GET_OBJECT_SUGGESTED_FILENAME_EXTENSION_IMPL_REGISTER(__type) \
    getObjectSuggestedFilenameExtensionImpl<__type>::AutoRegisterType getObjectSuggestedFilenameExtensionImpl<__type>::s_autoregister;

#define GET_OBJECT_SUGGESTED_FILENAME_EXTENSION_IMPL_DECLARE_AND_REGISTER(__type) \
    GET_OBJECT_SUGGESTED_FILENAME_EXTENSION_IMPL_DECLARE(__type) \
    GET_OBJECT_SUGGESTED_FILENAME_EXTENSION_IMPL_REGISTER(__type)


template<template<typename> class ImplT>
class getObjectFilenameFiltersImplBaseT : public functionImplBase
{
public:
    struct TypePolicy {
        struct CallParamType
        {
            CallParamType(SGIPluginHostInterface * hostInterface_, const SGIItemBase * item_)
                : hostInterface(hostInterface_), item(item_) {}
            SGIPluginHostInterface * hostInterface;
            const SGIItemBase * item;
        };
        typedef std::vector<std::string> ResultType;
    };

    getObjectFilenameFiltersImplBaseT(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL)
        : functionImplBase(hostInterface, item) {}

    typedef details::call_by_type_registryT<TypePolicy> Registry;
    template<typename T>
    class AutoRegisterTypeT : public details::call_by_type_auto_registerT<getObjectFilenameFiltersImplBaseT, T>
    {
    };

    typedef typename TypePolicy::CallParamType CallParamType;
    typedef typename TypePolicy::ResultType ResultType;

    template<typename T>
    static bool callImpl(const details::TypeWrap<T> & t, const CallParamType & param, ResultType & result)
    {
        ImplT<T> f(param.hostInterface, param.item);
        result = f.process();
        return !result.empty();
    }

    static bool call(SGIPluginHostInterface * hostInterface, const SGIItemBase * item, std::vector<std::string> & result)
    {
        CallParamType param(hostInterface, item);
        return Registry::instance()->call(*item->typeInfo(), param, result);
    }
};

#define GET_OBJECT_FILENAME_FILTERS_IMPL_TEMPLATE() \
    template<typename T> \
    class getObjectFilenameFiltersImpl { \
    public: \
        typedef sgi::details::constexpr_false accept; \
        getObjectFilenameFiltersImpl(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL) {} \
        std::vector<std::string> process() { return std::vector<std::string>(); } \
    }; \
    typedef getObjectFilenameFiltersImplBaseT<getObjectFilenameFiltersImpl> getObjectFilenameFiltersImplBase;

#define GET_OBJECT_FILENAME_FILTERS_IMPL_DECLARE(__type) \
    template<> \
    class getObjectFilenameFiltersImpl<__type> : public getObjectFilenameFiltersImplBase { \
    private: \
        typedef AutoRegisterTypeT<__type> AutoRegisterType; \
        static AutoRegisterType s_autoregister; \
    public: \
        typedef sgi::details::constexpr_true accept; \
        getObjectFilenameFiltersImpl<__type>(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL) \
            : getObjectFilenameFiltersImplBase(hostInterface, item) {} \
        std::vector<std::string> process(); \
    };

#define GET_OBJECT_FILENAME_FILTERS_IMPL_REGISTER(__type) \
    getObjectFilenameFiltersImpl<__type>::AutoRegisterType getObjectFilenameFiltersImpl<__type>::s_autoregister;

#define GET_OBJECT_FILENAME_FILTERS_IMPL_DECLARE_AND_REGISTER(__type) \
    GET_OBJECT_FILENAME_FILTERS_IMPL_DECLARE(__type) \
    GET_OBJECT_FILENAME_FILTERS_IMPL_REGISTER(__type)

template<template<typename> class ImplT>
class getObjectPathImplBaseT : public functionImplBase
{
public:
    struct TypePolicy {
        struct CallParamType
        {
            CallParamType(SGIPluginHostInterface * hostInterface_, const SGIItemBase * item_)
                : hostInterface(hostInterface_), item(item_) {}
            SGIPluginHostInterface * hostInterface;
            const SGIItemBase * item;
        };
        typedef SGIItemBasePtrPath ResultType;
    };

    getObjectPathImplBaseT(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL)
        : functionImplBase(hostInterface, item) {}

    typedef details::call_by_type_registryT<TypePolicy> Registry;
    template<typename T>
    class AutoRegisterTypeT : public details::call_by_type_auto_registerT<getObjectPathImplBaseT, T>
    {
    };

    typedef typename TypePolicy::CallParamType CallParamType;
    typedef typename TypePolicy::ResultType ResultType;

    template<typename T>
    static bool callImpl(const details::TypeWrap<T> & t, const CallParamType & param, ResultType & result)
    {
        ImplT<T> f(param.hostInterface, param.item);
        result = f.process();
        return !result.empty();
    }

    static bool call(SGIPluginHostInterface * hostInterface, const SGIItemBase * item, SGIItemBasePtrPath & result)
    {
        CallParamType param(hostInterface, item);
        return Registry::instance()->call(*item->typeInfo(), param, result);
    }
};

#define GET_OBJECT_PATH_IMPL_TEMPLATE() \
    template<typename T> \
    class getObjectPathImpl { \
    public: \
        typedef sgi::details::constexpr_false accept; \
        getObjectPathImpl(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL) {} \
        SGIItemBasePtrPath process() { return SGIItemBasePtrPath(); } \
    }; \
    typedef getObjectPathImplBaseT<getObjectPathImpl> getObjectPathImplBase;

#define GET_OBJECT_PATH_IMPL_DECLARE(__type) \
    template<> \
    class getObjectPathImpl<__type> : public getObjectPathImplBase { \
    private: \
        typedef AutoRegisterTypeT<__type> AutoRegisterType; \
        static AutoRegisterType s_autoregister; \
    public: \
        typedef sgi::details::constexpr_true accept; \
        getObjectPathImpl<__type>(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL) \
            : getObjectPathImplBase(hostInterface, item) {} \
        SGIItemBasePtrPath process(); \
    };

#define GET_OBJECT_PATH_IMPL_REGISTER(__type) \
    getObjectPathImpl<__type>::AutoRegisterType getObjectPathImpl<__type>::s_autoregister;

#define GET_OBJECT_PATH_IMPL_DECLARE_AND_REGISTER(__type) \
    GET_OBJECT_PATH_IMPL_DECLARE(__type) \
    GET_OBJECT_PATH_IMPL_REGISTER(__type)

template<template<typename> class ImplT>
class hasCallbackImplBaseT : public functionImplBase
{
public:
    struct TypePolicy {
        struct CallParamType
        {
            CallParamType(SGIPluginHostInterface * hostInterface_, const SGIItemBase * item_)
                : hostInterface(hostInterface_), item(item_) {}
            SGIPluginHostInterface * hostInterface;
            const SGIItemBase * item;
        };
        typedef bool ResultType;
    };

    hasCallbackImplBaseT(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL)
        : functionImplBase(hostInterface, item) {}

    typedef details::call_by_type_registryT<TypePolicy> Registry;
    template<typename T>
    class AutoRegisterTypeT : public details::call_by_type_auto_registerT<hasCallbackImplBaseT, T>
    {
    };

    typedef typename TypePolicy::CallParamType CallParamType;
    typedef typename TypePolicy::ResultType ResultType;

    template<typename T>
    static bool callImpl(const details::TypeWrap<T> & t, const CallParamType & param, ResultType & result)
    {
        ImplT<T> f(param.hostInterface, param.item);
        result = f.process();
        return result;
    }

    static bool call(SGIPluginHostInterface * hostInterface, const SGIItemBase * item, bool & result)
    {
        CallParamType param(hostInterface, item);
        return Registry::instance()->call(*item->typeInfo(), param, result);
    }

    bool callNextHandler()
    {
        bool ret = false;
        SGIItemBase * itemNext = _item->nextBase();
        if(itemNext)
        {
            bool result;
            if(call(_hostInterface, itemNext, result))
                ret = result;
        }
        return ret;
    }

};

#define HAS_CALLBACK_IMPL_TEMPLATE() \
    template<typename T> \
    class hasCallbackImpl { \
    public: \
        typedef sgi::details::constexpr_false accept; \
        hasCallbackImpl(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL) {} \
        bool process() { return false; } \
    }; \
    typedef hasCallbackImplBaseT<hasCallbackImpl> hasCallbackImplBase;

#define HAS_CALLBACK_IMPL_DECLARE(__type) \
    template<> \
    class hasCallbackImpl<__type> : public hasCallbackImplBase { \
    private: \
        typedef AutoRegisterTypeT<__type> AutoRegisterType; \
        static AutoRegisterType s_autoregister; \
    public: \
        typedef sgi::details::constexpr_true accept; \
        hasCallbackImpl<__type>(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL) \
            : hasCallbackImplBase(hostInterface, item) {} \
        bool process(); \
    };

#define HAS_CALLBACK_IMPL_REGISTER(__type) \
    hasCallbackImpl<__type>::AutoRegisterType hasCallbackImpl<__type>::s_autoregister;

#define HAS_CALLBACK_IMPL_DECLARE_AND_REGISTER(__type) \
    HAS_CALLBACK_IMPL_DECLARE(__type) \
    HAS_CALLBACK_IMPL_REGISTER(__type)


template<template<typename> class ImplT>
class writeObjectFileImplBaseT : public functionImplBase
{
public:
    struct TypePolicy {
        struct CallParamType
        {
            CallParamType(SGIPluginHostInterface * hostInterface_, SGIItemBase * item_, const std::string & filename_, const SGIItemBase* options_)
                : hostInterface(hostInterface_), item(item_), filename(filename_), options(options_) {}
            SGIPluginHostInterface * hostInterface;
            SGIItemBase * item;
            std::string filename;
            const SGIItemBase* options;
        };
        typedef bool ResultType;
    };

    writeObjectFileImplBaseT(SGIPluginHostInterface * hostInterface=NULL, SGIItemBase * item=NULL)
        : functionImplBase(hostInterface, item) {}

    typedef details::call_by_type_registryT<TypePolicy> Registry;
    template<typename T>
    class AutoRegisterTypeT : public details::call_by_type_auto_registerT<writeObjectFileImplBaseT, T>
    {
    };

    typedef typename TypePolicy::CallParamType CallParamType;
    typedef typename TypePolicy::ResultType ResultType;

    template<typename T>
    static bool callImpl(const details::TypeWrap<T> & t, const CallParamType & param, ResultType & result)
    {
        ImplT<T> f(param.hostInterface, param.item);
        result = f.process(param.filename, param.options);
        return result;
    }

    static bool call(SGIPluginHostInterface * hostInterface, SGIItemBase * item, const std::string & filename, const SGIItemBase* options, bool & result)
    {
        CallParamType param(hostInterface, item, filename, options);
        return Registry::instance()->call(*item->typeInfo(), param, result);
    }
};

#define WRITE_OBJECT_FILE_IMPL_TEMPLATE() \
    template<typename T> \
    class writeObjectFileImpl { \
    public: \
        typedef sgi::details::constexpr_false accept; \
        writeObjectFileImpl(SGIPluginHostInterface * hostInterface=NULL, SGIItemBase * item=NULL) {} \
        bool process(const std::string & filename, const SGIItemBase* options=NULL) { return false; } \
    }; \
    typedef writeObjectFileImplBaseT<writeObjectFileImpl> writeObjectFileImplBase;

#define WRITE_OBJECT_FILE_IMPL_DECLARE(__type) \
    template<> \
    class writeObjectFileImpl<__type> : public writeObjectFileImplBase { \
    private: \
        typedef AutoRegisterTypeT<__type> AutoRegisterType; \
        static AutoRegisterType s_autoregister; \
    public: \
        typedef sgi::details::constexpr_true accept; \
        writeObjectFileImpl<__type>(SGIPluginHostInterface * hostInterface=NULL, SGIItemBase * item=NULL) \
            : writeObjectFileImplBase(hostInterface, item) {} \
        bool process(const std::string & filename, const SGIItemBase* options=NULL); \
    };

#define WRITE_OBJECT_FILE_IMPL_REGISTER(__type) \
    writeObjectFileImpl<__type>::AutoRegisterType writeObjectFileImpl<__type>::s_autoregister;

#define WRITE_OBJECT_FILE_IMPL_DECLARE_AND_REGISTER(__type) \
    WRITE_OBJECT_FILE_IMPL_DECLARE(__type) \
    WRITE_OBJECT_FILE_IMPL_REGISTER(__type)

template<template<typename> class ImplT>
class objectTreeBuildImplBaseT : public functionImplBase
{
public:
    struct TypePolicy {
        struct CallParamType
        {
            CallParamType(SGIPluginHostInterface * hostInterface_, SGIItemBase * item_, IObjectTreeItem * treeItem_)
                : hostInterface(hostInterface_), item(item_), treeItem(treeItem_) {}
            SGIPluginHostInterface * hostInterface;
            SGIItemBase * item;
            IObjectTreeItem * treeItem;
        };
        typedef bool ResultType;
    };

    objectTreeBuildImplBaseT(SGIPluginHostInterface * hostInterface=NULL, SGIItemBase * item=NULL)
        : functionImplBase(hostInterface, item) {}

    typedef details::call_by_type_registryT<TypePolicy> Registry;
    template<typename T>
    class AutoRegisterTypeT : public details::call_by_type_auto_registerT<objectTreeBuildImplBaseT, T>
    {
    };

    typedef typename TypePolicy::CallParamType CallParamType;
    typedef typename TypePolicy::ResultType ResultType;

    template<typename T>
    static bool callImpl(const details::TypeWrap<T> & t, const CallParamType & param, ResultType & result)
    {
        ImplT<T> f(param.hostInterface, param.item);
        result = f.build(param.treeItem);
        return result;
    }

    static bool call(SGIPluginHostInterface * hostInterface, SGIItemBase * item, IObjectTreeItem * treeItem)
    {
        CallParamType param(hostInterface, item, treeItem);
        bool result;
        if(!Registry::instance()->call(*item->typeInfo(), param, result))
            return false;
        else
            return result;
    }

    bool callNextHandler(IObjectTreeItem * treeItem)
    {
        bool ret = false;
        SGIItemBase * itemNext = _item->nextBase();
        if(itemNext)
            ret = _hostInterface->objectTreeBuildTree(treeItem, itemNext);
        return ret;
    }
};

#define OBJECT_TREE_BUILD_IMPL_TEMPLATE() \
    template<typename T> \
    class objectTreeBuildImpl { \
    public: \
        typedef sgi::details::constexpr_false accept; \
        objectTreeBuildImpl(SGIPluginHostInterface * hostInterface=NULL, SGIItemBase * item=NULL) {} \
        bool build(IObjectTreeItem * treeItem) { return false; } \
    }; \
    typedef objectTreeBuildImplBaseT<objectTreeBuildImpl> objectTreeBuildImplBase;

#define OBJECT_TREE_BUILD_IMPL_DECLARE(__type) \
    template<> \
    class objectTreeBuildImpl<__type> : public objectTreeBuildImplBase { \
    private: \
        typedef AutoRegisterTypeT<__type> AutoRegisterType; \
        static AutoRegisterType s_autoregister; \
    public: \
        typedef sgi::details::constexpr_true accept; \
        objectTreeBuildImpl<__type>(SGIPluginHostInterface * hostInterface=NULL, SGIItemBase * item=NULL) \
            : objectTreeBuildImplBase(hostInterface, item) {} \
        bool build(IObjectTreeItem * treeItem); \
    };

#define OBJECT_TREE_BUILD_IMPL_REGISTER(__type) \
    objectTreeBuildImpl<__type>::AutoRegisterType objectTreeBuildImpl<__type>::s_autoregister;

#define OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(__type) \
    OBJECT_TREE_BUILD_IMPL_DECLARE(__type) \
    OBJECT_TREE_BUILD_IMPL_REGISTER(__type)

template<template<typename> class ImplT>
class objectTreeBuildRootImplBaseT : public functionImplBase
{
public:
    struct TypePolicy {
        struct CallParamType
        {
            CallParamType(SGIPluginHostInterface * hostInterface_, SGIItemBase * item_, IObjectTreeItem * treeItem_)
                : hostInterface(hostInterface_), item(item_), treeItem(treeItem_) {}
            SGIPluginHostInterface * hostInterface;
            SGIItemBase * item;
            IObjectTreeItem * treeItem;
        };
        typedef bool ResultType;
    };

    objectTreeBuildRootImplBaseT(SGIPluginHostInterface * hostInterface=NULL, SGIItemBase * item=NULL)
        : functionImplBase(hostInterface, item) {}

    typedef details::call_by_type_registryT<TypePolicy> Registry;
    template<typename T>
    class AutoRegisterTypeT : public details::call_by_type_auto_registerT<objectTreeBuildRootImplBaseT, T>
    {
    };

    typedef typename TypePolicy::CallParamType CallParamType;
    typedef typename TypePolicy::ResultType ResultType;

    template<typename T>
    static bool callImpl(const details::TypeWrap<T> & t, const CallParamType & param, ResultType & result)
    {
        ImplT<T> f(param.hostInterface, param.item);
        result = f.build(param.treeItem);
        return result;
    }

    static bool call(SGIPluginHostInterface * hostInterface, SGIItemBase * item, IObjectTreeItem * treeItem)
    {
        CallParamType param(hostInterface, item, treeItem);
        bool result;
        if(!Registry::instance()->call(*item->typeInfo(), param, result))
            return false;
        else
            return result;
    }
};

#define OBJECT_TREE_BUILD_ROOT_IMPL_TEMPLATE() \
    template<typename T> \
    class objectTreeBuildRootImpl { \
    public: \
        typedef sgi::details::constexpr_false accept; \
        objectTreeBuildRootImpl(SGIPluginHostInterface * hostInterface=NULL, SGIItemBase * item=NULL) {} \
        bool build(IObjectTreeItem * treeItem) { return false; } \
    }; \
    typedef objectTreeBuildRootImplBaseT<objectTreeBuildRootImpl> objectTreeBuildRootImplBase;

#define OBJECT_TREE_BUILD_ROOT_IMPL_DECLARE(__type) \
    template<> \
    class objectTreeBuildRootImpl<__type> : public objectTreeBuildRootImplBase { \
    private: \
        typedef AutoRegisterTypeT<__type> AutoRegisterType; \
        static AutoRegisterType s_autoregister; \
    public: \
        typedef sgi::details::constexpr_true accept; \
        objectTreeBuildRootImpl<__type>(SGIPluginHostInterface * hostInterface=NULL, SGIItemBase * item=NULL) \
            : objectTreeBuildRootImplBase(hostInterface, item) {} \
        bool build(IObjectTreeItem * treeItem); \
    };

#define OBJECT_TREE_BUILD_ROOT_IMPL_REGISTER(__type) \
    objectTreeBuildRootImpl<__type>::AutoRegisterType objectTreeBuildRootImpl<__type>::s_autoregister;

#define OBJECT_TREE_BUILD_ROOT_IMPL_DECLARE_AND_REGISTER(__type) \
    OBJECT_TREE_BUILD_ROOT_IMPL_DECLARE(__type) \
    OBJECT_TREE_BUILD_ROOT_IMPL_REGISTER(__type)

template<template<typename> class ImplT>
class contextMenuPopulateImplBaseT : public functionImplBase
{
public:
    struct TypePolicy {
        struct CallParamType
        {
            CallParamType(SGIPluginHostInterface * hostInterface_, SGIItemBase * item_, IContextMenuItem * menuItem_)
                : hostInterface(hostInterface_), item(item_), menuItem(menuItem_) {}
            SGIPluginHostInterface * hostInterface;
            SGIItemBase * item;
            IContextMenuItem * menuItem;
        };
        typedef bool ResultType;
    };

    contextMenuPopulateImplBaseT(SGIPluginHostInterface * hostInterface=NULL, SGIItemBase * item=NULL)
        : functionImplBase(hostInterface, item) {}

    typedef details::call_by_type_registryT<TypePolicy> Registry;
    template<typename T>
    class AutoRegisterTypeT : public details::call_by_type_auto_registerT<contextMenuPopulateImplBaseT, T>
    {
    };

    typedef typename TypePolicy::CallParamType CallParamType;
    typedef typename TypePolicy::ResultType ResultType;

    template<typename T>
    static bool callImpl(const details::TypeWrap<T> & t, const CallParamType & param, ResultType & result)
    {
        ImplT<T> f(param.hostInterface, param.item);
        result = f.populate(param.menuItem);
        return result;
    }

    static bool call(SGIPluginHostInterface * hostInterface, SGIItemBase * item, IContextMenuItem * menuItem)
    {
        CallParamType param(hostInterface, item, menuItem);
        bool result;
        if(!Registry::instance()->call(*item->typeInfo(), param, result))
            return false;
        else
            return result;
    }
    bool callNextHandler(IContextMenuItem * menuItem)
    {
        bool ret = false;
        SGIItemBase * itemNext = _item->nextBase();
        if(itemNext)
            ret = _hostInterface->contextMenuPopulate(menuItem, itemNext);
        return ret;
    }
};

#define CONTEXT_MENU_POPULATE_IMPL_TEMPLATE() \
    template<typename T> \
    class contextMenuPopulateImpl { \
    public: \
        typedef sgi::details::constexpr_false accept; \
        contextMenuPopulateImpl(SGIPluginHostInterface * hostInterface=NULL, SGIItemBase * item=NULL) {}\
        bool populate(IContextMenuItem * menuItem) { return false; } \
    }; \
    typedef contextMenuPopulateImplBaseT<contextMenuPopulateImpl> contextMenuPopulateImplBase;

#define CONTEXT_MENU_POPULATE_IMPL_DECLARE(__type) \
    template<> \
    class contextMenuPopulateImpl<__type> : public contextMenuPopulateImplBase { \
    private: \
        typedef AutoRegisterTypeT<__type> AutoRegisterType; \
        static AutoRegisterType s_autoregister; \
    public: \
        typedef sgi::details::constexpr_true accept; \
        contextMenuPopulateImpl<__type>(SGIPluginHostInterface * hostInterface=NULL, SGIItemBase * item=NULL) \
            : contextMenuPopulateImplBase(hostInterface, item) {} \
        bool populate(IContextMenuItem * menuItem); \
    };

#define CONTEXT_MENU_POPULATE_IMPL_REGISTER(__type) \
    contextMenuPopulateImpl<__type>::AutoRegisterType contextMenuPopulateImpl<__type>::s_autoregister;

#define CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(__type) \
    CONTEXT_MENU_POPULATE_IMPL_DECLARE(__type) \
    CONTEXT_MENU_POPULATE_IMPL_REGISTER(__type)


template<template<typename> class ImplT>
class guiAdapterParentWidgetImplBaseT : public functionImplBase
{
public:
    struct TypePolicy {
        struct CallParamType
        {
            CallParamType(SGIPluginHostInterface * hostInterface_, QWidgetPtr * widget_, SGIItemBase * item_)
                : hostInterface(hostInterface_), widget(widget_), item(item_) {}
            SGIPluginHostInterface * hostInterface;
            QWidgetPtr * widget;
            SGIItemBase * item;
        };
        typedef bool ResultType;
    };

    guiAdapterParentWidgetImplBaseT(SGIPluginHostInterface * hostInterface=NULL, QWidgetPtr * widget=NULL, SGIItemBase * item=NULL)
        : functionImplBase(hostInterface, item)
        , _widget(widget)
        {}

    typedef details::call_by_type_registryT<TypePolicy> Registry;
    template<typename T>
    class AutoRegisterTypeT : public details::call_by_type_auto_registerT<guiAdapterParentWidgetImplBaseT, T>
    {
    };

    typedef typename TypePolicy::CallParamType CallParamType;
    typedef typename TypePolicy::ResultType ResultType;

    template<typename T>
    static bool callImpl(const details::TypeWrap<T> & t, const CallParamType & param, ResultType & result)
    {
        ImplT<T> f(param.hostInterface, param.widget, param.item);
        result = f.execute();
        return result;
    }

    static bool call(SGIPluginHostInterface * hostInterface, QWidgetPtr * widget, SGIItemBase * item)
    {
        CallParamType param(hostInterface, widget, item);
        bool result;
        if(!Registry::instance()->call(*item->typeInfo(), param, result))
            return false;
        else
            return result;
    }
protected:
    QWidgetPtr * _widget;
};


#define GUI_ADAPTER_PARENT_WIDGET_IMPL_TEMPLATE() \
    template<typename T> \
    class guiAdapterParentWidgetImpl { \
    public: \
        typedef sgi::details::constexpr_false accept; \
        guiAdapterParentWidgetImpl(SGIPluginHostInterface * hostInterface=NULL, QWidgetPtr * widget=NULL, SGIItemBase * item=NULL) {} \
        bool execute() { return false; } \
    }; \
    typedef guiAdapterParentWidgetImplBaseT<guiAdapterParentWidgetImpl> guiAdapterParentWidgetImplBase;

#define GUI_ADAPTER_PARENT_WIDGET_IMPL_DECLARE(__type) \
    template<> \
    class guiAdapterParentWidgetImpl<__type> : public guiAdapterParentWidgetImplBase { \
    private: \
        typedef AutoRegisterTypeT<__type> AutoRegisterType; \
        static AutoRegisterType s_autoregister; \
    public: \
        typedef sgi::details::constexpr_true accept; \
        guiAdapterParentWidgetImpl<__type>(SGIPluginHostInterface * hostInterface=NULL, QWidgetPtr * widget=NULL, SGIItemBase * item=NULL) \
            : guiAdapterParentWidgetImplBase(hostInterface, widget, item) {} \
        bool execute(); \
    };

#define GUI_ADAPTER_PARENT_WIDGET_IMPL_REGISTER(__type) \
    guiAdapterParentWidgetImpl<__type>::AutoRegisterType guiAdapterParentWidgetImpl<__type>::s_autoregister;

#define GUI_ADAPTER_PARENT_WIDGET_IMPL_DECLARE_AND_REGISTER(__type) \
    GUI_ADAPTER_PARENT_WIDGET_IMPL_DECLARE(__type) \
    GUI_ADAPTER_PARENT_WIDGET_IMPL_REGISTER(__type)

template<template<typename> class ImplT>
class guiAdapterSetViewImplBaseT : public functionImplBase
{
public:
    struct TypePolicy {
        struct CallParamType
        {
            CallParamType(SGIPluginHostInterface * hostInterface_, SGIItemBase * view_, const SGIItemBase * item_, double animationTime_)
                : hostInterface(hostInterface_), view(view_), item(item_), animationTime(animationTime_) {}
            SGIPluginHostInterface * hostInterface;
            SGIItemBase * view;
            const SGIItemBase * item;
            double animationTime;
        };
        typedef bool ResultType;
    };

    guiAdapterSetViewImplBaseT(SGIPluginHostInterface * hostInterface=NULL, SGIItemBase * view=NULL, const SGIItemBase * targetItem=NULL, double animationTime=0)
        : functionImplBase(hostInterface, view)
        , _targetItem(targetItem)
        , _animationTime(animationTime) {}

    typedef details::call_by_type_registryT<TypePolicy> Registry;
    template<typename T>
    class AutoRegisterTypeT : public details::call_by_type_auto_registerT<guiAdapterSetViewImplBaseT, T>
    {
    };

    typedef typename TypePolicy::CallParamType CallParamType;
    typedef typename TypePolicy::ResultType ResultType;

    template<typename T>
    static bool callImpl(const details::TypeWrap<T> & t, const CallParamType & param, ResultType & result)
    {
        ImplT<T> f(param.hostInterface, param.view, param.item, param.animationTime);
        result = f.execute();
        return result;
    }

    static bool call(SGIPluginHostInterface * hostInterface, SGIItemBase * view, const SGIItemBase * item, double animationTime)
    {
        CallParamType param(hostInterface, view, item, animationTime);
        bool result;
        if(!Registry::instance()->call(*view->typeInfo(), param, result))
            return false;
        else
            return result;
    }

    template<typename T, typename ItemType >
    inline const T * getTargetObject() const
    {
        return getTargetObject<T, ItemType, DynamicCaster>();
    }
    template<typename T, typename ItemType, template<typename, typename> class CasterT>
    const T * getTargetObject() const
    {
        if(!_targetItem.valid())
            return NULL;
        typedef typename ItemType::ObjectType ObjectType;
        const ObjectType * obj = static_cast<const ItemType*>(_targetItem.get())->object();
        const T * ret = CasterT<const T, ObjectType>::cast(obj);
        return ret;
    }
protected:
    osg::ref_ptr<const SGIItemBase> _targetItem;
    double _animationTime;
};


#define GUI_ADAPTER_SET_VIEW_IMPL_TEMPLATE() \
    template<typename T> \
    class guiAdapterSetViewImpl { \
    public: \
        typedef sgi::details::constexpr_false accept; \
        guiAdapterSetViewImpl(SGIPluginHostInterface * hostInterface=NULL, SGIItemBase * view=NULL, const SGIItemBase * item=NULL, double animationTime=0) {} \
        bool execute() { return false; } \
    }; \
    typedef guiAdapterSetViewImplBaseT<guiAdapterSetViewImpl> guiAdapterSetViewImplBase;

#define GUI_ADAPTER_SET_VIEW_IMPL_DECLARE(__type) \
    template<> \
    class guiAdapterSetViewImpl<__type> : public guiAdapterSetViewImplBase { \
    private: \
        typedef AutoRegisterTypeT<__type> AutoRegisterType; \
        static AutoRegisterType s_autoregister; \
    public: \
        typedef sgi::details::constexpr_true accept; \
        guiAdapterSetViewImpl<__type>(SGIPluginHostInterface * hostInterface=NULL, SGIItemBase * view=NULL, const SGIItemBase * item=NULL, double animationTime=0) \
            : guiAdapterSetViewImplBase(hostInterface, view, item, animationTime) {} \
        bool execute(); \
    };

#define GUI_ADAPTER_SET_VIEW_IMPL_REGISTER(__type) \
    guiAdapterSetViewImpl<__type>::AutoRegisterType guiAdapterSetViewImpl<__type>::s_autoregister;

#define GUI_ADAPTER_SET_VIEW_IMPL_DECLARE_AND_REGISTER(__type) \
    GUI_ADAPTER_SET_VIEW_IMPL_DECLARE(__type) \
    GUI_ADAPTER_SET_VIEW_IMPL_REGISTER(__type)

template<template<typename> class ImplT>
class getOrCreateObjectLoggerImplBaseT : public functionImplBase
{
public:
    struct TypePolicy {
        struct CallParamType
        {
            CallParamType(SGIPluginHostInterface * hostInterface_, const SGIItemBase * item_, bool createIfNotExists_)
                : hostInterface(hostInterface_), item(item_), createIfNotExists(createIfNotExists_) {}
            SGIPluginHostInterface * hostInterface;
            const SGIItemBase * item;
            bool createIfNotExists;
        };
        typedef IObjectLoggerPtr & ResultType;
    };

    getOrCreateObjectLoggerImplBaseT(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL, bool createIfNotExists=false)
        : functionImplBase(hostInterface, item), _createIfNotExists(createIfNotExists) {}

    typedef details::call_by_type_registryT<TypePolicy> Registry;
    template<typename T>
    class AutoRegisterTypeT : public details::call_by_type_auto_registerT<getOrCreateObjectLoggerImplBaseT, T>
    {
    };

    typedef typename TypePolicy::CallParamType CallParamType;
    typedef typename TypePolicy::ResultType ResultType;

    template<typename T>
    static bool callImpl(const details::TypeWrap<T> & t, const CallParamType & param, ResultType & result)
    {
        ImplT<T> f(param.hostInterface, param.item, param.createIfNotExists);
        return f.execute(result);
    }

    static bool call(SGIPluginHostInterface * hostInterface, const SGIItemBase * item, IObjectLoggerPtr & result, bool createIfNotExists)
    {
        CallParamType param(hostInterface, item, createIfNotExists);
        return Registry::instance()->call(*item->typeInfo(), param, result);
    }
protected:
    bool _createIfNotExists;
};

#define GET_OR_CREATE_OBJECTLOGGER_IMPL_TEMPLATE() \
    template<typename T> \
    class getOrCreateObjectLoggerImpl { \
    public: \
        typedef sgi::details::constexpr_false accept; \
        getOrCreateObjectLoggerImpl(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL, bool createIfNotExists=false) {} \
        bool execute(IObjectLoggerPtr & result) { return false; } \
    }; \
    typedef getOrCreateObjectLoggerImplBaseT<getOrCreateObjectLoggerImpl> getOrCreateObjectLoggerImplBase;

#define GET_OR_CREATE_OBJECTLOGGER_IMPL_DECLARE(__type) \
    template<> \
    class getOrCreateObjectLoggerImpl<__type> : public getOrCreateObjectLoggerImplBase { \
    private: \
        typedef AutoRegisterTypeT<__type> AutoRegisterType; \
        static AutoRegisterType s_autoregister; \
    public: \
        typedef sgi::details::constexpr_true accept; \
        getOrCreateObjectLoggerImpl<__type>(SGIPluginHostInterface * hostInterface=NULL, const SGIItemBase * item=NULL, bool createIfNotExists=false) \
            : getOrCreateObjectLoggerImplBase(hostInterface, item, createIfNotExists) {} \
        bool execute(IObjectLoggerPtr & result); \
    };

#define GET_OR_CREATE_OBJECTLOGGER_IMPL_REGISTER(__type) \
    getOrCreateObjectLoggerImpl<__type>::AutoRegisterType getOrCreateObjectLoggerImpl<__type>::s_autoregister;

#define GET_OR_CREATE_OBJECTLOGGER_IMPL_DECLARE_AND_REGISTER(__type) \
    GET_OR_CREATE_OBJECTLOGGER_IMPL_DECLARE(__type) \
    GET_OR_CREATE_OBJECTLOGGER_IMPL_REGISTER(__type) \

} // namespace sgi
