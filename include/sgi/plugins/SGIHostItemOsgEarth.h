// kate: syntax C++;
// SGI - Copyright (C) 2012-2018 FAST Protect, Andreas Roth

#pragma once
#include "SGIHostItemBase.h"
#include <osgEarth/Config>

namespace osg {
    class Referenced;
}

namespace osgEarth {
    class Config;
    class ConfigOptions;
}

namespace sgi {

template<typename TYPE>
class SGIHostItemCopyImpl : public SGIHostItemBase
{
public:
    typedef TYPE ItemInfoType;
    typedef typename TYPE::ObjectType ObjectType;
    typedef typename TYPE::ObjectStorageType ObjectStorageType;
    enum AllocationMode {
        NO_DELETE,
        USE_NEW_DELETE
    };

    explicit SGIHostItemCopyImpl(ObjectType * object, details::Referenced * userData=nullptr, unsigned flags=0)
        : SGIHostItemBase(userData, flags), _object(object), _allocated(false)
        {
        }
    explicit SGIHostItemCopyImpl(const ObjectType * object, details::Referenced * userData=nullptr, unsigned flags=0)
        : SGIHostItemBase(userData, flags), _object(const_cast<ObjectType*>(object)), _allocated(false)
        {
        }
    SGIHostItemCopyImpl(const ObjectType & object, details::Referenced * userData=nullptr, unsigned flags=0)
        : SGIHostItemBase(userData, flags), _object(new ObjectType(object)), _allocated(true)
        {
        }
    explicit SGIHostItemCopyImpl(AllocationMode allocationMode, ObjectType * object, details::Referenced * userData=nullptr, unsigned flags=0)
        : SGIHostItemBase(userData, flags), _object(object), _allocated(allocationMode==NO_DELETE?false:true)
        {}

    SGIHostItemCopyImpl(const SGIHostItemCopyImpl & rhs)
        : SGIHostItemBase(rhs), _object(rhs._object)
    {
    }
    ~SGIHostItemCopyImpl() override
    {
        if(_allocated)
            delete _object;
    }
    ObjectType * object() { return TYPE::objectPtr(_object); }
    ObjectType * object() const { return TYPE::objectPtr(_object); }

    template<typename USER_DATA_TYPE>
    USER_DATA_TYPE * objectAs()
    {
        return dynamic_cast<USER_DATA_TYPE *>(object());
    }
    template<typename USER_DATA_TYPE>
    const USER_DATA_TYPE * objectAs() const
    {
        return dynamic_cast<const USER_DATA_TYPE *>(object());
    }

    template<typename USER_DATA_TYPE>
    USER_DATA_TYPE * userData()
    {
        return dynamic_cast<USER_DATA_TYPE *>(SGIHostItemBase::userDataPtr());
    }
    template<typename USER_DATA_TYPE>
    const USER_DATA_TYPE * userData() const
    {
        return dynamic_cast<const USER_DATA_TYPE *>(SGIHostItemBase::userDataPtr());
    }

    bool hasObject() const { return TYPE::objectPtr(_object) != nullptr; }
protected:
    ObjectStorageType _object;
    bool _allocated;
};

typedef SGIItemInfoPlainPtr<osgEarth::Config> SGIItemInfoOsgEarthConfig;
typedef SGIHostItemCopyImpl<SGIItemInfoOsgEarthConfig> SGIHostItemOsgEarthConfig;
typedef SGIItemInfoPlainPtr<osgEarth::ConfigOptions> SGIItemInfoOsgEarthConfigOptions;
typedef SGIHostItemCopyImpl<SGIItemInfoOsgEarthConfigOptions> SGIHostItemOsgEarthConfigOptions;


template<typename HOST_ITEM_TYPE, typename HOLDER_TYPE>
class SGIItemCopyT : public SGIItemBase
{
public:
    typedef typename HOLDER_TYPE::ObjectType ObjectType;
    typedef HOST_ITEM_TYPE HostItemType;

    SGIItemCopyT(const HostItemType * /*hostItem*/=nullptr, SGIItemType type=SGIItemTypeInvalid, ObjectType * object=nullptr, unsigned flags=0, unsigned score=0, details::Referenced * userData=nullptr)
        : SGIItemBase(object ? new HOLDER_TYPE(object) : nullptr, type, flags, score, userData) {}
    SGIItemCopyT(const HostItemType * /*hostItem*/, SGIItemType type, ObjectType & object, unsigned flags=0, unsigned score=0, details::Referenced * userData=nullptr)
        : SGIItemBase(new HOLDER_TYPE(object), type, flags, score, userData) {}
    SGIItemCopyT(SGIItemType type, ObjectType * object=nullptr, unsigned flags=0, unsigned score=0, details::Referenced * userData=nullptr)
        : SGIItemBase(object ? new HOLDER_TYPE(object) : nullptr, type, flags, score, userData) {}
    SGIItemCopyT(const SGIItemCopyT & rhs)
        : SGIItemBase(rhs) {}
    ~SGIItemCopyT() override
        { }

    SGI_Object(sgi, SGIItemCopyT)

    ObjectType * object() { return static_cast<HOLDER_TYPE*>(_holder.get())->object(); }
    ObjectType * object() const { return static_cast<const HOLDER_TYPE*>(_holder.get())->object(); }

    template<typename USER_DATA_TYPE>
    USER_DATA_TYPE * objectAs()
    {
        return dynamic_cast<USER_DATA_TYPE *>(object());
    }
    template<typename USER_DATA_TYPE>
    const USER_DATA_TYPE * objectAs() const
    {
        return dynamic_cast<const USER_DATA_TYPE *>(object());
    }
};

} // namespace sgi
