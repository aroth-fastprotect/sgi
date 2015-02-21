#pragma once
#include <osg/Object>
#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <vector>
#include <typeinfo>

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
    SGIItemTypeStateSetEffective,
    SGIItemTypeStateSetModeList,
    SGIItemTypeStateSetAttributeList,
    SGIItemTypeStateSetTextureModeList,
    SGIItemTypeStateSetTextureAttributeLists,
    SGIItemTypeStateSetUniformList,
    SGIItemTypeParentalNodePath,
    SGIItemTypeDrawables,
    SGIItemTypeDrawable,
    SGIItemTypeDrawableColors,
    SGIItemTypeDrawableColorIndicies,
    SGIItemTypeDrawableNormals,
    SGIItemTypeDrawableNormalIndicies,
    SGIItemTypeDrawableVertices,
    SGIItemTypeDrawableVertexIndicies,
    SGIItemTypeDrawableTexCoords,
    SGIItemTypeDrawableTexCoordsList,
    SGIItemTypeDrawableTexCoordIndicies,
    SGIItemTypeDrawablePrimitiveSetList,
    SGIItemTypeImageLayers,
    SGIItemTypeElevationLayers,
    SGIItemTypeModelLayers,
    SGIItemTypeMaskLayers,
    SGIItemTypeImageLayer,
    SGIItemTypeElevationLayer,
    SGIItemTypeModelLayer,
    SGIItemTypeMaskLayer,
    SGIItemTypeTerrain,
    SGIItemTypeMap,
    SGIItemTypeMapNode,
    SGIItemTypeStatistics,
    SGIItemTypeCallbacks,
    SGIItemTypeGraphicsContextCameras,
    SGIItemTypeGraphicsContextOperations,
    SGIItemTypeGraphicsContextGLExtensions,
    SGIItemTypeModel,
    SGIItemTypeModelRaw,
    SGIItemTypeModelType,
    SGIItemTypeModelObject,
    SGIItemTypeModelObjectProperty,
    SGIItemTypeLocationTrack,
    SGIItemTypeLocationFutureTrack,
    SGIItemTypeDigitalSensorDevices,
    SGIItemTypeRelayDevices,
    SGIItemTypeCameras,
    SGIItemTypeCamerasAsync,
    SGIItemTypePlacemarks,
    SGIItemTypeGlobalPlacemarks,
    SGIItemTypeGroupPlacemarks,
    SGIItemTypeUserPlacemarks,
    SGIItemTypeTemporaryPlacemarks,
    SGIItemTypeBeings,
    SGIItemTypeCommBeings,
    SGIItemTypeAlarms,
    SGIItemTypePointOfInterests,
    SGIItemTypeLayers,
    SGIItemTypeVicinities,
    SGIItemTypeBuildings,
    SGIItemTypeFloors,
    SGIItemTypeRooms,
    SGIItemTypeSky,
    SGIItemTypeView,
    SGIItemTypeImage,
    SGIItemTypeShaders,
    SGIItemTypeShaderSource,
    SGIItemTypeShaderCodeInjectionMap,
    SGIItemTypeVirtualProgramShaderMap,
    SGIItemTypeVirtualProgramFunctions,
    SGIItemTypePopulated,
    SGIItemTypeFixLocations,
    SGIItemTypeVisibleLocations,
    SGIItemTypeString,
    SGIItemTypeWatcherEntries,
    SGIItemTypeWatcherNodes,
    SGIItemTypePickerHits,
    SGIItemTypePickerHit,
    SGIItemTypeOptions,
    SGIItemTypeCachedNodes,
    SGIItemTypeSettings,
    SGIItemTypeImageLayerOptions,
    SGIItemTypeElevationLayerOptions,
    SGIItemTypeModelLayerOptions,
    SGIItemTypeMaskLayerOptions,
    SGIItemTypeDataExtents,
    SGIItemTypeSlaves,
    SGIItemTypeConfig,
    SGIItemTypeTour,
    SGIItemTypeObjectLogger,
    SGIItemTypeArrayData,
    SGIItemTypeEventHandlers,
    SGIItemTypeDevices,
    SGIItemTypeLocationTypes,
    SGIItemTypeCommDisplayTypes,
    SGIItemTypeTrackingDevices,
    SGIItemTypeRadarDevices,
    SGIItemTypePolygonTypes,
    SGIItemTypeControlPorts,
    SGIItemTypeDataMaps,
    SGIItemTypeWidgets,
    SGIItemTypeActivePagedLODs,
    SGIItemTypeThreads,
    SGIItemTypeTileCache,
    SGIItemTypeTileCacheLRU,
    SGIItemTypeTileCacheMap,
    SGIItemTypeUpdatableNodeBaseRequired,
    SGIItemTypeUpdatableNodeBaseNotify,
    SGIItemTypeZones,
    SGIItemTypeParentCullState,
    SGIItemTypeStateAttibutes,
    SGIItemTypeStateSets,
    SGIItemTypeViewerBaseScenes,
    SGIItemTypeViewerBaseViews,
    SGIItemTypeViewerBaseCameras,
    SGIItemTypeViewerBaseContexts,
    SGIItemTypeLeaves,
    SGIItemTypeShaderComposerShaderMap,
    SGIItemTypeShaderComposerProgramFunctions,
    SGIItemTypeRenderInfoStateSetStack,
    SGIItemTypeRenderInfoRenderBinStack,
    SGIItemTypeRenderInfoCameraStack,
    SGIItemTypeRenderInfoAppliedProgramSet,
    SGIItemTypeRenderInfoState,
    SGIItemTypeSymbols,
    SGIItemTypeReadersWriters,
    SGIItemTypeImageProcessors,
    SGIItemTypeDatabaseRevisions,
    SGIItemTypeBufferDatas,
    SGIItemTypePendingChild,
    SGIItemTypeTasks,
    SGIItemTypeMethods,
    SGIItemTypeCameraFrustum,
    SGIItemTypeCameraVideoProjection,
    SGIItemTypeCameraPolygon,
    SGIItemTypeChannels,
    SGIItemTypeAnimations,
    SGIItemTypeSerializers,
    SGIItemTypeWrappers,
    SGIItemTypeCompressors,
    SGIItemTypeDBPagerFileRequests,
    SGIItemTypeDBPagerHttpRequests,
    SGIItemTypeDBPagerDataToCompile,
    SGIItemTypeDBPagerDataToMerge
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
class SGIPluginInfo;

class ISGIPluginInfo
{
public:
    virtual unsigned pluginScore() const = 0;
};

class SGIItemBase : public osg::Object
{
public:
    SGIItemBase(SGIItemType type=SGIItemTypeInvalid, unsigned flags=0, unsigned score=0, osg::Referenced * userData=NULL)
        : osg::Object(), _type(type), _flags(flags), _score(score), _type_info(NULL)
        , _pluginInfo(NULL), _next(NULL), _prev(), _number(0), _userData(userData) {}
    SGIItemBase(const SGIItemBase & rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
        : osg::Object(rhs, copyop), _type(rhs._type), _flags(rhs._flags), _score(rhs._score), _type_info(rhs._type_info)
        , _pluginInfo(rhs._pluginInfo), _next(rhs._next), _prev(rhs._prev), _number(rhs._number), _userData(rhs._userData) {}
    ~SGIItemBase()
        { }

    META_Object(sgi, SGIItemBase);

    SGIItemBase & operator = (const SGIItemBase & rhs)
    {
        _type = rhs._type;
        _flags = rhs._flags;
        _score = rhs._score;
        _type_info = rhs._type_info;
        _pluginInfo = rhs._pluginInfo;
        _next = rhs._next;
        _prev = rhs._prev;
        _number = rhs._number;
        _userData = rhs._userData;
        return *this;
    }
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

    SGIItemBase * rootBase() const
    {
        SGIItemBasePtr current = const_cast<SGIItemBase *>(this);
        SGIItemBasePtr parent;
        do
        {
            if(current->_prev.lock(parent) && parent.valid())
                current = parent;
        }
        while(parent.valid());
        return current.release();
    }
    void insertAfter(SGIItemBase * item)
    {
        SGIItemBasePtr next_prev;
        if(_next.valid())
        {
            next_prev = _next->_prev;
            _next->_prev = item;
        }
        item->_next = this->_next;
        item->_prev = this;
        this->_next = item;
    }
    void insertBefore(SGIItemBase * item)
    {
        SGIItemBasePtr prev_next;
        if(_prev.valid())
        {
            prev_next = _prev->_next;
            _prev->_next = item;
        }
        item->_prev = this->_prev;
        item->_next = this;
        this->_prev = item;
    }
    void insertByScore(SGIItemBase * item, SGIItemBasePtr & front);
    bool isListValid() const;
    size_t listSize() const;

    template<typename ANOTHER_ITEMTYPE>
    ANOTHER_ITEMTYPE * clone(SGIItemType newType=SGIItemTypeInvalid, const osg::CopyOp & copyop=osg::CopyOp::SHALLOW_COPY)
    {
        SGIItemBasePtr ret;
        SGIItemBasePtr previous_cloned;
        SGIItemBasePtr current = this;
        SGIItemBasePtr safe_this = this;
        while(current.valid())
        {
            SGIItemBasePtr clonedItem = (SGIItemBase*)current->clone(copyop);
            if(newType!=SGIItemTypeInvalid)
                clonedItem->setType(newType);
            if(!ret.valid())
            {
                // we always return the first cloned item
                ret = clonedItem;
            }
            if(previous_cloned.valid())
            {
                previous_cloned->_next = clonedItem;
                clonedItem->_prev = previous_cloned;
            }
            // remember the item cloned in the last loop
            previous_cloned = clonedItem;
            current = current->nextBase();
        }
        return static_cast<ANOTHER_ITEMTYPE *>(ret.release());
    }

    template<typename ANOTHER_ITEMTYPE>
    ANOTHER_ITEMTYPE * clone(SGIItemType newType, unsigned number, const osg::CopyOp & copyop=osg::CopyOp::SHALLOW_COPY)
    {
        SGIItemBasePtr ret;
        SGIItemBasePtr previous_cloned;
        SGIItemBasePtr current = this;
        while(current.valid())
        {
            SGIItemBasePtr clonedItem = (SGIItemBase*)current->clone(copyop);
            if(newType!=SGIItemTypeInvalid)
                clonedItem->setType(newType);
            clonedItem->setNumber(number);
            if(!ret.valid())
            {
                // we always return the first cloned item
                ret = clonedItem;
            }
            if(previous_cloned.valid())
            {
                previous_cloned->_next = clonedItem;
                clonedItem->_prev = previous_cloned;
            }
            // remember the item cloned in the last loop
            previous_cloned = clonedItem;
            current = current->nextBase();
        }
        return static_cast<ANOTHER_ITEMTYPE *>(ret.release());
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
    virtual int compare(const SGIItemBase & rhs) const
    {
        if(rhs._type == _type)
            return 0;
        else if(rhs._type < _type)
            return -1;
        else
            return 1;
    }

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

template <class To, class From>
struct StaticCaster
{
    static To* cast(From* obj)
    {
        return static_cast<To*>(obj);
    }
    static const To* cast(const From* obj)
    {
        return static_cast<const To*>(obj);
    }
};

template <class To, class From>
struct DynamicCaster
{
    static To* cast(From* obj)
    {
        return dynamic_cast<To*>(obj);
    }
    static const To* cast(const From* obj)
    {
        return dynamic_cast<const To*>(obj);
    }
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

class Image : public osg::Referenced
{
public:
    enum ImageFormat {
        ImageFormatInvalid = -1,
        ImageFormatRGB = 0,
        ImageFormatARGB,
        ImageFormatFloat
    };
    Image(ImageFormat format=ImageFormatInvalid, void * data=NULL, size_t length=0, unsigned width=0, unsigned height=0, unsigned depth=0)
        : _format(format), _data(data), _length(length), _width(width), _height(height), _depth(depth) {}
    Image(const Image & rhs)
        : _format(rhs._format), _data(rhs._data), _length(rhs._length), _width(rhs._width), _height(rhs._height), _depth(rhs._depth) {}
    Image & operator=(const Image & rhs)
        {
            _format = rhs._format;
            _data = rhs._data;
            _length = rhs._length;
            _width = rhs._width;
            _height = rhs._height;
            _depth = rhs._depth;
            return *this;
        }

    ImageFormat format() const { return _format; }
    const void * data() const { return _data; }
    void * data() { return _data; }
    size_t length() const { return _length; }
    unsigned width() const { return _width; }
    unsigned height() const { return _height; }
    unsigned depth() const { return _depth; }

protected:
    ImageFormat _format;
    void * _data;
    size_t _length;
    unsigned _width;
    unsigned _height;
    unsigned _depth;
};

} // namespace sgi
