// kate: syntax C++11;
// SGI - Copyright (C) 2012-2016 FAST Protect, Andreas Roth

#pragma once
#include <osg/Object>
#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <vector>
#include <typeinfo>
#include <sgi/Export>

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
typedef osg::ref_ptr<SGIItemBase> SGIItemBasePtr;
typedef osg::observer_ptr<SGIItemBase> SGIItemBaseOverserverPtr;
typedef std::vector<SGIItemBasePtr> SGIItemBasePtrPath;
typedef std::vector<SGIItemBasePtr> SGIItemBasePtrVector;
class SGIHostItemBase;
class SGIPluginInfo;

class Image;
typedef osg::ref_ptr<Image> ImagePtr;
typedef osg::ref_ptr<const Image> ConstImagePtr;

class ISGIPluginInfo
{
public:
    virtual unsigned pluginScore() const = 0;
};

class SGI_EXPORT SGIItemBase : public osg::Object
{
public:
    static unsigned getTotalItemCount();
public:
    SGIItemBase(SGIItemType type=SGIItemTypeInvalid, unsigned flags=0, unsigned score=0, osg::Referenced * userData=NULL);
    SGIItemBase(const SGIItemBase & rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);
    virtual ~SGIItemBase();

    META_Object(sgi, SGIItemBase);

    SGIItemBase & operator = (const SGIItemBase & rhs);
    bool operator == (const SGIItemBase & rhs) const
    {
        return compare(rhs) == 0;
    }
    bool operator != (const SGIItemBase & rhs) const
    {
        return compare(rhs) != 0;
    }

    bool valid() const { return _type != SGIItemTypeInvalid; }
    SGIItemType type() const { return _type; }
    void setType(SGIItemType type) { _type = type; }
    unsigned score() const { return _score; }
    unsigned incrementScore() { return ++_score; }
    unsigned decrementScore() { return --_score; }

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

    SGIItemBase * rootBase() const;
    void insertAfter(SGIItemBase * item);
    void insertBefore(SGIItemBase * item);
    void insertByScore(SGIItemBase * item, SGIItemBasePtr & front);
    bool isListValid() const;
    size_t listSize() const;

    template<typename ANOTHER_ITEMTYPE>
    ANOTHER_ITEMTYPE * clone(SGIItemType newType=SGIItemTypeInvalid, const osg::CopyOp & copyop=osg::CopyOp::SHALLOW_COPY)
    {
        return static_cast<ANOTHER_ITEMTYPE *>(cloneImpl(newType, copyop));
    }

    template<typename ANOTHER_ITEMTYPE>
    ANOTHER_ITEMTYPE * clone(SGIItemType newType, unsigned number, const osg::CopyOp & copyop=osg::CopyOp::SHALLOW_COPY)
    {
        return static_cast<ANOTHER_ITEMTYPE *>(cloneImpl(newType, number, copyop));
    }

    template<typename ANOTHER_ITEMTYPE>
    ANOTHER_ITEMTYPE * clone(SGIItemType newType, osg::Referenced * userData, const osg::CopyOp & copyop=osg::CopyOp::SHALLOW_COPY)
    {
        return static_cast<ANOTHER_ITEMTYPE *>(cloneImpl(newType, userData, copyop));
    }

    template<typename ANOTHER_ITEMTYPE>
    ANOTHER_ITEMTYPE * clone(SGIItemType newType, unsigned number, osg::Referenced * userData, const osg::CopyOp & copyop=osg::CopyOp::SHALLOW_COPY)
    {
        return static_cast<ANOTHER_ITEMTYPE *>(cloneImpl(newType, number, userData, copyop));
    }

    SGIItemBase * previousBase() const { return _prev.get(); }
    SGIItemBase * nextBase() const { return _next.get(); }
    template<typename ITEMTYPE>
    ITEMTYPE * next() const { return dynamic_cast<ITEMTYPE*>(_next.get()); }
    void setNext(SGIItemBase * next) { _next = next; }

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

protected:
    virtual int compare(const SGIItemBase & rhs) const;

    SGIItemBase * cloneImpl(SGIItemType newType, const osg::CopyOp & copyop);
    SGIItemBase * cloneImpl(SGIItemType newType, unsigned number, const osg::CopyOp & copyop);
    SGIItemBase * cloneImpl(SGIItemType newType, osg::Referenced * userData, const osg::CopyOp & copyop);
    SGIItemBase * cloneImpl(SGIItemType newType, unsigned number, osg::Referenced * userData, const osg::CopyOp & copyop);

protected:
    SGIItemType             _type;
    unsigned                _flags;
private:
    unsigned                _score;
    const ISGIPluginInfo *  _pluginInfo;
    const std::type_info *  _type_info;
    SGIItemBasePtr          _next;
    SGIItemBaseOverserverPtr _prev;
    unsigned                _number;
    osg::ref_ptr<osg::Referenced> _userData;
};

namespace internal {
    class ReferencedAccess : public osg::Referenced {
    public:
        int getRefCount() const { return _refCount; }
    };
}

template<typename T>
int getRefCount(const T * ref) {
    return static_cast<const internal::ReferencedAccess*>(static_cast<const osg::Referenced*>(ref))->getRefCount();
}

template<typename T>
class ReferencedDataT : public osg::Referenced
{
public:
    ReferencedDataT()
        : osg::Referenced()
        , _data()
        {}
    ReferencedDataT(const T & data)
        : osg::Referenced()
        , _data(data)
        {}
    ReferencedDataT(const ReferencedDataT & rhs)
        : osg::Referenced(rhs)
        , _data(rhs._data)
        {}
    const T & data() const { return _data; }
    T & data() { return _data; }

    operator const T & () const {return _data;}
    operator T & () {return _data;}
private:
    T _data;
};

typedef ReferencedDataT<int> ReferencedDataInt;
typedef std::pair<int, int> IntPair;
typedef ReferencedDataT<IntPair> ReferencedDataIntPair;
typedef ReferencedDataT<std::string> ReferencedDataString;

template<typename T>
class ReferencedRawPtrT : public osg::Referenced
{
public:
    ReferencedRawPtrT()
        : osg::Referenced()
        , _ptr(NULL)
        {}
    ReferencedRawPtrT(T * ptr)
        : osg::Referenced()
        , _ptr(ptr)
        {}
    ReferencedRawPtrT(const ReferencedRawPtrT & rhs)
        : osg::Referenced(rhs)
        , _ptr(rhs._ptr)
        {}

    T& operator*() const { return *_ptr; }
    T* operator->() const { return _ptr; }
    T* get() const { return _ptr; }

    bool operator!() const   { return _ptr==0; } // not required
    bool valid() const       { return _ptr!=0; }

private:
    T* _ptr;
};

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

class SGI_EXPORT Quat {
public:
    typedef double value_type;

    value_type  _v[4];    // a four-vector

    inline Quat() { _v[0]=0.0; _v[1]=0.0; _v[2]=0.0; _v[3]=1.0; }

    inline Quat( value_type x, value_type y, value_type z, value_type w )
    {
        _v[0]=x;
        _v[1]=y;
        _v[2]=z;
        _v[3]=w;
    }

    inline bool valid() const { return !isNaN(); }
    bool isNaN() const;

    inline value_type & operator [] (int i) { return _v[i]; }
    inline value_type   operator [] (int i) const { return _v[i]; }

    inline value_type & x() { return _v[0]; }
    inline value_type & y() { return _v[1]; }
    inline value_type & z() { return _v[2]; }
    inline value_type & w() { return _v[3]; }

    inline value_type x() const { return _v[0]; }
    inline value_type y() const { return _v[1]; }
    inline value_type z() const { return _v[2]; }
    inline value_type w() const { return _v[3]; }

    /** return true if the Quat represents a zero rotation, and therefore can be ignored in computations.*/
    bool zeroRotation() const { return _v[0]==0.0 && _v[1]==0.0 && _v[2]==0.0 && _v[3]==1.0; }

    /// Length of the quaternion = sqrt( vec . vec )
    value_type length() const;

    /// Length of the quaternion = vec . vec
    value_type length2() const
    {
        return _v[0]*_v[0] + _v[1]*_v[1] + _v[2]*_v[2] + _v[3]*_v[3];
    }
};

class SGI_EXPORT Matrix {

public:
    typedef double value_type;
    value_type _mat[4][4];

    inline Matrix() { makeIdentity(); }
    inline Matrix( const Matrix& mat) { set(mat.ptr()); }
    inline explicit Matrix( float const * const ptr ) { set(ptr); }
    inline explicit Matrix( double const * const ptr ) { set(ptr); }
    inline explicit Matrix( const Quat& quat ) { makeRotate(quat); }

    Matrix(value_type a00, value_type a01, value_type a02, value_type a03,
            value_type a10, value_type a11, value_type a12, value_type a13,
            value_type a20, value_type a21, value_type a22, value_type a23,
            value_type a30, value_type a31, value_type a32, value_type a33);

    inline Matrix& operator = (const Matrix& rhs)
    {
        if( &rhs == this ) return *this;
        set(rhs.ptr());
        return *this;
    }

    inline bool valid() const { return !isNaN(); }
    bool isNaN() const;

    inline void set(const Matrix& rhs) { set(rhs.ptr()); }

    inline void set(float const * const ptr)
    {
        value_type* local_ptr = (value_type*)_mat;
        for(int i=0;i<16;++i) local_ptr[i]=(value_type)ptr[i];
    }

    inline void set(double const * const ptr)
    {
        value_type* local_ptr = (value_type*)_mat;
        for(int i=0;i<16;++i) local_ptr[i]=(value_type)ptr[i];
    }

    inline value_type& operator()(int row, int col) { return _mat[row][col]; }
    inline value_type operator()(int row, int col) const { return _mat[row][col]; }

    void set(value_type a00, value_type a01, value_type a02,value_type a03,
                value_type a10, value_type a11, value_type a12,value_type a13,
                value_type a20, value_type a21, value_type a22,value_type a23,
                value_type a30, value_type a31, value_type a32,value_type a33);

    value_type * ptr() { return (value_type*)_mat; }
    const value_type * ptr() const { return (const value_type *)_mat; }

    bool isIdentity() const
    {
        return _mat[0][0]==1.0 && _mat[0][1]==0.0 && _mat[0][2]==0.0 &&  _mat[0][3]==0.0 &&
                _mat[1][0]==0.0 && _mat[1][1]==1.0 && _mat[1][2]==0.0 &&  _mat[1][3]==0.0 &&
                _mat[2][0]==0.0 && _mat[2][1]==0.0 && _mat[2][2]==1.0 &&  _mat[2][3]==0.0 &&
                _mat[3][0]==0.0 && _mat[3][1]==0.0 && _mat[3][2]==0.0 &&  _mat[3][3]==1.0;
    }

    void makeIdentity();
    void makeRotate( const Quat& );

    void setRotate(const Quat& q);
};

enum MatrixUsage {
    MatrixUsageUnknown = 0,
    MatrixUsageModel,
    MatrixUsageView,
    MatrixUsageProjection,
};

} // namespace sgi
