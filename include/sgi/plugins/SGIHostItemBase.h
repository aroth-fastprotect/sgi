// kate: syntax C++;
// SGI - Copyright (C) 2012-2018 FAST Protect, Andreas Roth

#pragma once
#include "SGIItemBase.h"
#include "SGIItemInfo"

namespace sgi {

class SGIHostItemBase;
typedef details::ref_ptr<SGIHostItemBase> SGIHostItemBasePtr;

class SGIHostItemBase : public details::Referenced
{
public:
    SGIHostItemBase(details::Referenced * userData=nullptr, unsigned flags=0)
        : _flags(flags), _userData(userData) {}
    SGIHostItemBase(const SGIHostItemBase & rhs)
        : details::Referenced(rhs), _flags(rhs._flags), _userData(rhs._userData)
    {
    }
    virtual ~SGIHostItemBase() {}

    template<typename HOSTITEMTYPE>
    HOSTITEMTYPE * as()
    {
        return dynamic_cast<HOSTITEMTYPE*>(this);
    }
    template<typename HOSTITEMTYPE>
    const HOSTITEMTYPE * as() const
    {
        return dynamic_cast<const HOSTITEMTYPE*>(this);
    }

    void setFlags(unsigned newFlags) { _flags = newFlags; }
    unsigned flags() const { return _flags; }
    void setFlag(SGIItemFlags flag, bool onOrOff) { _flags = (onOrOff)?(_flags | (unsigned)flag):(_flags | ~((unsigned)flag)); }
    bool hasFlag(unsigned flag) const { return (_flags & flag) != 0; }
    bool hasNotFlag(unsigned flag) const { return (_flags & flag) == 0; }

    void setUserData(details::Referenced * userData)
    {
        _userData = userData;
    }
    details::Referenced * userDataPtr()
    {
        return _userData.get();
    }
    details::Referenced * userDataPtr() const
    {
        return _userData.get();
    }

private:
    unsigned       _flags;
    SGIUserDataPtr _userData;
};

template<typename TYPE>
class SGIHostItemImpl : public SGIHostItemBase
{
public:
    typedef TYPE ItemInfoType;
    typedef typename TYPE::ObjectType ObjectType;
    typedef typename TYPE::ObjectStorageType ObjectStorageType;

    SGIHostItemImpl(ObjectType * object, details::Referenced * userData=nullptr, unsigned flags=0)
        : SGIHostItemBase(userData, flags), _object(object)
        {
        }
    SGIHostItemImpl(const ObjectType * object, details::Referenced * userData=nullptr, unsigned flags=0)
        : SGIHostItemBase(userData, flags), _object(const_cast<ObjectType*>(object))
        {
        }
    SGIHostItemImpl(const ObjectType & object, details::Referenced * userData=nullptr, unsigned flags=0)
        : SGIHostItemBase(userData, flags), _object(const_cast<ObjectType*>(&object))
        {
        }
    SGIHostItemImpl(const SGIHostItemImpl & rhs)
        : SGIHostItemBase(rhs), _object(rhs._object)
    {
    }
    ~SGIHostItemImpl() override
    {
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
};

} // namespace sgi
