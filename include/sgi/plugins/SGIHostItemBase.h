// kate: syntax C++;
// SGI - Copyright (C) 2012-2018 FAST Protect, Andreas Roth

#pragma once
#include "SGIItemBase.h"

namespace sgi {

class SGIHostItemBase;
typedef osg::ref_ptr<SGIHostItemBase> SGIHostItemBasePtr;

class SGIHostItemBase : public osg::Referenced
{
public:
    SGIHostItemBase(osg::Referenced * userData=nullptr, unsigned flags=0)
        : _flags(flags), _userData(userData) {}
    SGIHostItemBase(const SGIHostItemBase & rhs)
        : osg::Referenced(rhs), _flags(rhs._flags), _userData(rhs._userData)
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

    void setUserData(osg::Referenced * userData)
    {
        _userData = userData;
    }
    template<typename USER_DATA_TYPE>
    USER_DATA_TYPE * userData()
    {
        return dynamic_cast<USER_DATA_TYPE *>(_userData.get());
    }
    template<typename USER_DATA_TYPE>
    const USER_DATA_TYPE * userData() const
    {
        return dynamic_cast<const USER_DATA_TYPE *>(_userData.get());
    }
    osg::Referenced * userDataPtr()
    {
        return _userData.get();
    }
    osg::Referenced * userDataPtr() const
    {
        return _userData.get();
    }

private:
    unsigned                _flags;
    osg::ref_ptr<osg::Referenced> _userData;
};

template<typename OBJECT_TYPE, typename OBJECT_STORE_TYPE>
class SGIHostItemImpl : public SGIHostItemBase
{
public:
    typedef OBJECT_TYPE ObjectType;
    typedef OBJECT_STORE_TYPE ObjectStorageType;
    SGIHostItemImpl(ObjectType * object, osg::Referenced * userData=nullptr, unsigned flags=0)
        : SGIHostItemBase(userData, flags), _object(object)
        {
        }
    SGIHostItemImpl(const ObjectType * object, osg::Referenced * userData=nullptr, unsigned flags=0)
        : SGIHostItemBase(userData, flags), _object(const_cast<ObjectType*>(object))
        {
        }
    SGIHostItemImpl(const SGIHostItemImpl & rhs)
        : SGIHostItemBase(rhs), _object(rhs._object)
    {
    }
    ~SGIHostItemImpl() override
    {
    }
    ObjectType * object() { return _object; }
    ObjectType * object() const { return _object; }
    bool hasObject() const { return _object != nullptr; }
protected:
    ObjectStorageType _object;
};

} // namespace sgi
