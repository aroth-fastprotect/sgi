// kate: syntax C++;
// SGI - Copyright (C) 2012-2018 FAST Protect, Andreas Roth

#pragma once
#include <sgi/Export>
#include <sgi/details/ref_ptr>
#include <sgi/details/observer_ptr>
#include <sgi/details/Object>
#include <vector>
#include <typeinfo>
#include "SGIItemInfo"

class QImage;

namespace sgi {

enum SGIItemType
{
    SGIItemTypeInvalid = 0,
    SGIItemTypeDummy,
    SGIItemTypeTreeRoot,
    SGIItemTypeObject,
    SGIItemTypeChilds,
    SGIItemTypeParents,
    SGIItemTypeObservers,
    SGIItemTypeStatistics,
    SGIItemTypeCallbacks,
    SGIItemTypeSettings,
    SGIItemTypeObjectLogger,
    SGIItemTypeProxyRealItem,
    SGIItemTypePlugins,
    SGIItemTypeBackendPlugins,

    SGIItemTypeDefaultPluginBase = 0x1000,
    SGIItemTypePluginBaseOsg = SGIItemTypeDefaultPluginBase + 0x0000,
    SGIItemTypePluginBaseOsgEarth = SGIItemTypeDefaultPluginBase + 0x1000,
    SGIItemTypePluginBaseQt = SGIItemTypeDefaultPluginBase + 0x2000,
    SGIItemTypePluginBaseLog4cplus = SGIItemTypeDefaultPluginBase + 0x3000,

    SGIItemTypeUserPluginBase = 0x10000,
};

enum SGIItemFlags {
    SGIItemFlagNone = 0x0000,
    SGIItemFlagContinue = 0x0001,
    SGIItemFlagRoot = 0x0002,
    SGIItemFlagAll = 0xffff
};

class SGIItemBase;
typedef details::ref_ptr<SGIItemBase> SGIItemBasePtr;
typedef details::observer_ptr<SGIItemBase> SGIItemBaseOverserverPtr;
typedef std::vector<SGIItemBasePtr> SGIItemBasePtrPath;
typedef std::vector<SGIItemBasePtr> SGIItemBasePtrVector;
typedef details::ref_ptr<details::Referenced> SGIUserDataPtr;

class SGIHostItemBase;
template<typename TYPE>
class SGIHostItemImpl;

class Image;
typedef details::ref_ptr<Image> ImagePtr;
typedef details::ref_ptr<const Image> ConstImagePtr;

class ISGIPluginInfo
{
public:
    virtual unsigned pluginScore() const = 0;
    virtual const std::string & pluginName() const = 0;
    virtual const std::string & pluginFilename() const = 0;
};

class SGI_IMPL_EXPORT SGIItemHolder : public details::Referenced
{
public:
    static unsigned getTotalItemCount();
public:
    ~SGIItemHolder() override;

    virtual int compare(const SGIItemHolder & rhs) const = 0;

protected:
    SGIItemHolder();
    SGIItemHolder(const SGIItemHolder & rhs);
};

template<typename TYPE>
class SGIItemHolderT : public SGIItemHolder
{
public:
    typedef typename TYPE::ObjectType ObjectType;
    typedef typename TYPE::ObjectStorageType ObjectStorageType;

    SGIItemHolderT(ObjectType * object)
        : SGIItemHolder(), _object(object), _copied(false)
    {
    }
    SGIItemHolderT(const ObjectType & object)
        : SGIItemHolder(), _object(TYPE::copyObject(object)), _copied(true)
    {
    }
    SGIItemHolderT(const SGIItemHolderT & rhs)
        : SGIItemHolder(rhs), _object(rhs._object), _copied(rhs._copied)
    {
    }
    ~SGIItemHolderT()
    {
        if(_copied)
            TYPE::deleteObject(_object);
    }
    int compare(const SGIItemHolder & rhs) const override
    {
        if(static_cast<const SGIItemHolderT&>(rhs)._object == _object)
            return 0;
        else if(static_cast<const SGIItemHolderT&>(rhs)._object < _object)
            return -1;
        else
            return 1;
    }

    ObjectType * object() { return TYPE::objectPtr(_object); }
    ObjectType * object() const { return TYPE::objectPtr(_object); }

private:
    ObjectStorageType _object;
    bool _copied;
};

class SGI_IMPL_EXPORT SGIItemBase : public details::Object
{
public:
    static unsigned getTotalItemCount();
protected:
    SGIItemBase(SGIItemHolder * holder=nullptr, SGIItemType type=SGIItemTypeInvalid, unsigned flags=0, unsigned score=0, details::Referenced * userData=nullptr);
    SGIItemBase(const SGIItemBase & rhs);
public:
    virtual ~SGIItemBase();

    SGI_Object(sgi, SGIItemBase)

    SGIItemBase & operator = (const SGIItemBase & rhs);
    bool operator == (const SGIItemBase & rhs) const;
    bool operator != (const SGIItemBase & rhs) const;

    bool valid() const;
    SGIItemType type() const;
    void setType(SGIItemType type);
    unsigned score() const;
    unsigned incrementScore();
    unsigned decrementScore();

    unsigned number() const { return _number; }
    void setNumber(unsigned num) { _number = num; }

    void setFlags(unsigned newFlags) { _flags = newFlags; }
    unsigned flags() const { return _flags; }
    void setFlag(SGIItemFlags flag, bool onOrOff) { _flags = (onOrOff)?(_flags | (unsigned)flag):(_flags | ~((unsigned)flag)); }
    bool hasFlag(unsigned flag) const { return (_flags & flag) != 0; }
    bool hasNotFlag(unsigned flag) const { return (_flags & flag) == 0; }

    const std::type_info * typeInfo() const { return _type_info; }
    void setTypeInfo(const std::type_info & ti) { _type_info = &ti; }
    size_t hash_code() const { return _type_info->hash_code(); }
    const char * typeName() const { return _type_info->name(); }

    const ISGIPluginInfo * pluginInfo() const { return _pluginInfo; }
    void setPluginInfo(const ISGIPluginInfo * pluginInfo);
    void overridePluginInfoForAllItems(const ISGIPluginInfo * pluginInfo);

    SGIItemBase * rootBase() const;
    void insertAfter(SGIItemBase * item);
    void insertBefore(SGIItemBase * item);
    void insertByScore(SGIItemBase * item, SGIItemBasePtr & front);
    bool isListValid() const;
    size_t listSize() const;

    template<typename ANOTHER_ITEMTYPE>
    ANOTHER_ITEMTYPE * clone(SGIItemType newType=SGIItemTypeInvalid)
    {
        return static_cast<ANOTHER_ITEMTYPE *>(cloneImpl(newType));
    }

    template<typename ANOTHER_ITEMTYPE>
    ANOTHER_ITEMTYPE * clone(SGIItemType newType, unsigned number)
    {
        return static_cast<ANOTHER_ITEMTYPE *>(cloneImpl(newType, number));
    }

    template<typename ANOTHER_ITEMTYPE>
    ANOTHER_ITEMTYPE * clone(SGIItemType newType, details::Referenced * userData)
    {
        return static_cast<ANOTHER_ITEMTYPE *>(cloneImpl(newType, userData));
    }

    template<typename ANOTHER_ITEMTYPE>
    ANOTHER_ITEMTYPE * clone(SGIItemType newType, unsigned number, details::Referenced * userData)
    {
        return static_cast<ANOTHER_ITEMTYPE *>(cloneImpl(newType, number, userData));
    }

    SGIItemBase * previousBase() const { return _prev.get(); }
    SGIItemBase * nextBase() const { return _next.get(); }
    template<typename ITEMTYPE>
    ITEMTYPE * next() const { return dynamic_cast<ITEMTYPE*>(_next.get()); }
    void setNext(SGIItemBase * next) { _next = next; }

    SGIItemHolder * holder() const { return _holder.get(); }

    template<typename ITEMTYPE>
    ITEMTYPE * as()
    {
        return dynamic_cast<ITEMTYPE*>(this);
    }
    template<typename ITEMTYPE>
    const ITEMTYPE * as() const
    {
        return dynamic_cast<const ITEMTYPE*>(this);
    }
    void setUserData(details::Referenced * userData)
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
    details::Referenced * userDataPtr()
    {
        return _userData.get();
    }
    details::Referenced * userDataPtr() const
    {
        return _userData.get();
    }

protected:
    int compare(const SGIItemBase & rhs) const;

    SGIItemBase * cloneImpl(SGIItemType newType);
    SGIItemBase * cloneImpl(SGIItemType newType, unsigned number);
    SGIItemBase * cloneImpl(SGIItemType newType, details::Referenced * userData);
    SGIItemBase * cloneImpl(SGIItemType newType, unsigned number, details::Referenced * userData);

protected:
    details::ref_ptr<SGIItemHolder> _holder;
    SGIItemType             _type;
    unsigned                _flags;
private:
    unsigned                _score;
    unsigned                _number;
    const ISGIPluginInfo *  _pluginInfo;
    const std::type_info *  _type_info;
    SGIItemBasePtr          _next;
    SGIItemBaseOverserverPtr _prev;
    SGIUserDataPtr          _userData;
};

template<typename HOST_ITEM_TYPE, typename HOLDER_TYPE>
class SGIItemT : public SGIItemBase
{
public:
    typedef typename HOLDER_TYPE::ObjectType ObjectType;
    typedef HOST_ITEM_TYPE HostItemType;

    SGIItemT(const HostItemType * /*hostItem*/=nullptr, SGIItemType type=SGIItemTypeInvalid, ObjectType * object=nullptr, unsigned flags=0, unsigned score=0, details::Referenced * userData=nullptr)
        : SGIItemBase(object ? new HOLDER_TYPE(object) : nullptr, type, flags, score, userData) {}
    SGIItemT(const HostItemType * /*hostItem*/, SGIItemType type, const ObjectType & object, unsigned flags=0, unsigned score=0, details::Referenced * userData=nullptr)
        : SGIItemBase(new HOLDER_TYPE(object), type, flags, score, userData) {}
    SGIItemT(SGIItemType type, ObjectType * object=nullptr, unsigned flags=0, unsigned score=0, details::Referenced * userData=nullptr)
        : SGIItemBase(object ? new HOLDER_TYPE(object) : nullptr, type, flags, score, userData) {}
    SGIItemT(const SGIItemT & rhs)
        : SGIItemBase(rhs) {}
    ~SGIItemT() override
        { }

    SGI_Object(sgi, SGIItemT)

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

namespace internal {
    class ReferencedAccess : public details::Referenced {
    public:
        int getRefCount() const { return _refCount; }
    };
}

template<typename T>
int getRefCount(const T * ref) {
    return static_cast<const internal::ReferencedAccess*>(static_cast<const details::Referenced*>(ref))->getRefCount();
}

struct Color {
    Color(float r_=0.0f, float g_=0.0f, float b_=0.0f, float a_=1.0f)
        : r(r_), g(g_), b(b_), a(a_) {}
    Color(int r_, int g_, int b_, int a_=255)
        : r((float)r_/255.0f), g((float)g_/255.0f), b((float)b_/255.0f), a((float)a_/255.0f) {}
    Color(const Color & rhs)
        : r(rhs.r), g(rhs.g), b(rhs.b), a(rhs.a) {}
    bool operator==(const Color & rhs) const
        { return (r==rhs.r && g==rhs.g && b==rhs.b && a==rhs.a); }
    bool operator!=(const Color & rhs) const
        { return (r!=rhs.r || g!=rhs.g || b!=rhs.b || a!=rhs.a); }
    Color & operator=(const Color & rhs)
        { r=rhs.r; g=rhs.g; b=rhs.b; a=rhs.a; return *this; }
    float r, g, b, a;
};

} // namespace sgi
