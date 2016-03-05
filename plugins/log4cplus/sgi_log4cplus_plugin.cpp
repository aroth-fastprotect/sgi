#include "stdafx.h"
#include <stdio.h>
#include <iostream>

#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SettingsDialog>
#include <sgi/plugins/SGIPluginImpl.h>
#include <sgi/plugins/SGIPluginInterface.h>
#include <sgi/plugins/SGIHostItemLog4cplus.h>
#include <sgi/plugins/SGIHostItemOsg.h>
#include <sgi/plugins/SGIPluginMacros.h>
#include <sgi/details/object_type_info>
#include <sgi/SGIItemInternal>

#include "SGIItemLog4cplus"
#include "ObjectTreeLog4cplus.h"
#include "writeHTMLLog4cplus.h"
#include "getObjectInfoLog4cplus.h"
#include "ContextMenuLog4cplus.h"
#include "MenuActionLog4cplus.h"

namespace sgi {

namespace log4cplus_plugin {

GENERATE_IMPL_TEMPLATE()
GENERATE_IMPL_NO_ACCEPT(osg::Referenced)

SGI_CALL_FUNCTION_FOR_OBJECT_TEMPLATE()

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(Log4cplusObjectBase, LOKI_TYPELIST(Log4cplusObjectLogger, Log4cplusObjectHierarchy, Log4cplusObjectAppender, Log4cplusObjectLayout))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(Log4cplusObjectLayout, LOKI_TYPELIST(Log4cplusObjectSimpleLayout, Log4cplusObjectPatternLayout))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Referenced, LOKI_TYPELIST(ISceneGraphDialog))

//LOKI_TYPELIST(SGIItemLog4cplus, SGIItemInternal),

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

template<typename ITEM_TYPE, template<typename> class generateItemAcceptImplTemplate>
struct generateSGIItem_BaseT
{
    typedef typename ITEM_TYPE::HostItemType HostItemType;
    typedef typename HostItemType::ObjectType ObjectType;
    static bool generate(const HostItemType * hostItem, SGIItemBasePtr & item)
    {
        typedef generateSGIItemT<ITEM_TYPE, generateItemAcceptImplTemplate> generateSGIItemFunctor;
        generateSGIItemFunctor func(hostItem);
        call_function_for_object_type<ObjectType>(hostItem->object(), func);
        bool ret = func.wasAccepted();
        if(ret)
            item = func.getItem();
        return ret;
    }
};

template<template<typename> class generateItemAcceptImplTemplate, typename ...ITEM_TYPE>
struct generateItemImplT
{
    typedef details::type_list<ITEM_TYPE...> item_types;
    struct call_proxy
    {
        const SGIHostItemBase * _hostItem;
        SGIItemBasePtr & _item;
        bool _ret;
        call_proxy(const SGIHostItemBase * hostItem, SGIItemBasePtr & item)
            : _hostItem(hostItem), _item(item), _ret(false) {}

        template<typename SINGLE_ITEM_TYPE>
        void operator()()
        {
            typedef typename SINGLE_ITEM_TYPE::HostItemType HostItemType;
            _ret = generateSGIItem_BaseT<SINGLE_ITEM_TYPE, generateItemAcceptImpl>::generate((const HostItemType*)_hostItem, _item);
        }
    };
    static bool generate(const SGIHostItemBase * object, SGIItemBasePtr & item)
    {
        call_proxy func(object, item);
        details::for_each_type<item_types>(func);
        return func._ret;
    }
};

typedef generateItemImplT<generateItemAcceptImpl, SGIItemLog4cplus, SGIItemInternal > generateItemImpl;


typedef SGIPluginImplementationT< generateItemImpl,
                                        writePrettyHTMLImpl,
                                        getObjectNameImpl,
                                        getObjectNameImpl,
                                        getObjectTypeImpl,
                                        getObjectPathImpl,
                                        defaultPluginGetObjectInfoStringImpl,
                                        defaultPluginGetObjectInfoStringImpl,
                                        defaultPluginGetObjectFilenameFiltersImpl,
                                        defaultPluginGetObjectHasCallbackImpl,
                                        defaultPluginWriteObjectFileImpl,
                                        objectTreeBuildImpl,
                                        objectTreeBuildRootImpl,
                                        contextMenuPopulateImpl,
                                        actionHandlerImpl
                                        >
    SGIPlugin_log4cplus_Implementation_base;

} // namespace log4cplus_plugin

class SGIPlugin_log4cplus_Implementation : public log4cplus_plugin::SGIPlugin_log4cplus_Implementation_base
{
public:
    SGIPlugin_log4cplus_Implementation(SGIPluginHostInterface * hostInterface=NULL)
        : log4cplus_plugin::SGIPlugin_log4cplus_Implementation_base(hostInterface)
    {
        SGIITEMTYPE_NAME(SGIItemTypeAppenders);
    }
    SGIPlugin_log4cplus_Implementation(const SGIPlugin_log4cplus_Implementation & rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
        : log4cplus_plugin::SGIPlugin_log4cplus_Implementation_base(rhs, copyop)
    {
    }

    META_Object(sgi_log4cplus, SGIPlugin_log4cplus_Implementation);
};

} // namespace sgi

SGI_PLUGIN_IMPLEMENT(log4cplus)
