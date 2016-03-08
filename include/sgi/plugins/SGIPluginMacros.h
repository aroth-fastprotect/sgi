// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#pragma once

#include "SGIItemBase.h"
#include "SGIPluginInterface.h"
#include <typeindex>
#include <unordered_map>
#include <string.h>

#include <sgi/details/caster>
#include <sgi/details/functionImplBase>

namespace sgi {

class SGIItemBase;
class SGIPluginHostInterface;
class IHostCallback;

template<template<typename> class ImplT>
class getObjectInfoFullStringImplBaseT : public details::functionImplBase
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
class getObjectInfoStringImplBaseT : public details::functionImplBase
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

#define GET_OBJECT_SUGGESTED_FILENAME_IMPL_DECLARE_AND_REGISTER(__type) \
    GET_OBJECT_SUGGESTED_FILENAME_IMPL_DECLARE(__type) \
    GET_OBJECT_SUGGESTED_FILENAME_IMPL_REGISTER(__type)

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
class getObjectFilenameFiltersImplBaseT : public details::functionImplBase
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
class getObjectPathImplBaseT : public details::functionImplBase
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
class hasCallbackImplBaseT : public details::functionImplBase
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
class writeObjectFileImplBaseT : public details::functionImplBase
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
class objectTreeBuildImplBaseT : public details::functionImplBase
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
class objectTreeBuildRootImplBaseT : public details::functionImplBase
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
class contextMenuPopulateImplBaseT : public details::functionImplBase
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
class guiAdapterParentWidgetImplBaseT : public details::functionImplBase
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
class guiAdapterSetViewImplBaseT : public details::functionImplBase
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
    template<typename T, typename ItemType, typename CasterT>
    const T * getTargetObject() const
    {
        if(!_targetItem.valid())
            return NULL;
        typedef typename ItemType::ObjectType ObjectType;
        const ObjectType * obj = static_cast<const ItemType*>(_targetItem.get())->object();
        const T * ret = CasterT::template cast<const T, const ObjectType>(obj);
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
class getOrCreateObjectLoggerImplBaseT : public details::functionImplBase
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
