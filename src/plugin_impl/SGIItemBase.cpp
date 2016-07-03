// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#include <sgi/plugins/SGIItemBase.h>
#include <sgi/plugins/SGIProxyItem.h>
#include <sstream>

#include <QImage>

namespace sgi {
// some method implementations from SGIItemBase which are only
// used within the SGI base and not in any plugin or caller.

namespace {
    static unsigned s_ItemCount = 0;
}

unsigned SGIItemBase::getTotalItemCount()
{
    return s_ItemCount;
}

SGIItemBase::SGIItemBase(SGIItemType type, unsigned flags, unsigned score, osg::Referenced * userData)
    : osg::Object(), _type(type), _flags(flags), _score(score), _type_info(NULL)
    , _pluginInfo(NULL), _next(NULL), _prev(), _number(0), _userData(userData)
{
    ++s_ItemCount;
}

SGIItemBase::SGIItemBase(const SGIItemBase & rhs, const osg::CopyOp& copyop)
    : osg::Object(rhs, copyop), _type(rhs._type), _flags(rhs._flags), _score(rhs._score), _type_info(rhs._type_info)
    , _pluginInfo(rhs._pluginInfo), _next(rhs._next), _prev(rhs._prev), _number(rhs._number), _userData(rhs._userData)
{
    ++s_ItemCount;
}

SGIItemBase::~SGIItemBase()
{
    --s_ItemCount;
}

SGIItemBase & SGIItemBase::operator = (const SGIItemBase & rhs)
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

int SGIItemBase::compare(const SGIItemBase & rhs) const
{
    if(rhs._type == _type)
        return 0;
    else if(rhs._type < _type)
        return -1;
    else
        return 1;
}


/// @brief override the plugin info in all items in the list
/// @param pluginInfo pointer to plugin info
void SGIItemBase::setPluginInfo(const ISGIPluginInfo * pluginInfo)
{
    SGIItemBasePtr item = this;
    while(item.valid())
    {
        item->_pluginInfo = pluginInfo;
        item = item->_next;
    }
}

/// @brief determines the length of the list
/// @note the returned list size does not include the current item
/// @return length of the list; zero if this item is the end of the list
size_t SGIItemBase::listSize() const
{
    size_t ret = 0;
    SGIItemBase * next = nextBase();
    while(next)
    {
        ret++;
        next = next->nextBase();
    }
    return ret;
}

SGIItemBase * SGIItemBase::rootBase() const
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

void SGIItemBase::insertAfter(SGIItemBase * item)
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

void SGIItemBase::insertBefore(SGIItemBase * item)
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

/// @brief inserts the given item (sorted by the score of the item)
/// @param item pointer to item to be inserted into the list
/// @return pointer to the head of the list (with the highest score)
void SGIItemBase::insertByScore(SGIItemBase * item, SGIItemBasePtr & front)
{
    front = this;
    SGIItemBasePtr itemToInsert = item;
    while(itemToInsert.valid())
    {
        SGIItemBasePtr nextItemToInsert = itemToInsert->_next;
        // drop the reference to it's predecessor to avoid circles
        if(nextItemToInsert.valid())
            nextItemToInsert->_prev = NULL;
        itemToInsert->_next = NULL;

        SGIItemBasePtr insertionPoint = front;
        while(insertionPoint->_score >= itemToInsert->_score)
        {
            if(insertionPoint->_score == itemToInsert->_score)
            {
                if( insertionPoint->_pluginInfo &&
                    itemToInsert->_pluginInfo &&
                    insertionPoint->_pluginInfo->pluginScore() > itemToInsert->_pluginInfo->pluginScore())
                {
                    if(insertionPoint->_next.valid())
                        insertionPoint = insertionPoint->_next;
                    else
                        break;
                }
                else
                    break;
            }
            else
            {
                if(insertionPoint->_next.valid())
                    insertionPoint = insertionPoint->_next;
                else
                    break;
            }
        }
        if(itemToInsert->_score == insertionPoint->_score)
        {
            if( insertionPoint->_pluginInfo &&
                itemToInsert->_pluginInfo &&
                itemToInsert->_pluginInfo->pluginScore() > insertionPoint->_pluginInfo->pluginScore())
            {
                insertionPoint->insertBefore(itemToInsert);
                if(insertionPoint == front)
                    front = itemToInsert;
            }
            else
                insertionPoint->insertAfter(itemToInsert);
        }
        else if(itemToInsert->_score > insertionPoint->_score)
        {
            insertionPoint->insertBefore(itemToInsert);
            if(insertionPoint == front)
                front = itemToInsert;
        }
        else
            insertionPoint->insertAfter(itemToInsert);
        itemToInsert = nextItemToInsert;
    }
}

/// @brief checks if the list is consistent
/// @return true if list is valid, otherwise false
bool SGIItemBase::isListValid() const
{
    bool ret = true;
    std::set<const SGIItemBase*> foundItems;
    const SGIItemBase * cur = this;
    while(cur)
    {
        if(foundItems.find(cur) != foundItems.end())
        {
            // found re-occurring item
            ret = false;
            break;
        }
        foundItems.insert(cur);
        const SGIItemBase * next = cur->_next.get();
        if(next)
        {
            if(!(next->score() <= cur->score()))
            {
                ret = false;
                break;
            }
        }
        cur = next;
    }
    return ret;
}

SGIItemBase * SGIItemBase::cloneImpl(SGIItemType newType, const osg::CopyOp & copyop)
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
    return ret.release();
}

SGIItemBase * SGIItemBase::cloneImpl(SGIItemType newType, unsigned number, const osg::CopyOp & copyop)
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
    return ret.release();
}

SGIItemBase * SGIItemBase::cloneImpl(SGIItemType newType, osg::Referenced * userData, const osg::CopyOp & copyop)
{
    SGIItemBasePtr ret;
    SGIItemBasePtr previous_cloned;
    SGIItemBasePtr current = this;
    while(current.valid())
    {
        SGIItemBasePtr clonedItem = (SGIItemBase*)current->clone(copyop);
        if(newType!=SGIItemTypeInvalid)
            clonedItem->setType(newType);
        clonedItem->setUserData(userData);
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
    return ret.release();
}

SGIItemBase * SGIItemBase::cloneImpl(SGIItemType newType, unsigned number, osg::Referenced * userData, const osg::CopyOp & copyop)
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
        clonedItem->setUserData(userData);
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
    return ret.release();
}

SGIProxyItemBase::SGIProxyItemBase(SGIPluginHostInterface * hostInterface, SGIItemBase * realItem)
    : _hostInterface(hostInterface), _realItem(realItem) 
{
}

SGIProxyItemBase::SGIProxyItemBase(SGIPluginHostInterface * hostInterface, 
                                    const std::string & name, 
                                    const std::string & typeName, 
                                    const std::string & displayName, 
                                    SGIItemBase * realItem)
    : _hostInterface(hostInterface), _name(name), _displayName(displayName), _typeName(typeName)
    , _realItem(realItem) 
{
}

SGIProxyItemBase::SGIProxyItemBase(const SGIProxyItemBase & rhs)
    : _hostInterface(rhs._hostInterface), _name(rhs._name), _displayName(rhs._displayName)
    , _typeName(rhs._typeName), _realItem(rhs._realItem) 
{
}

SGIItemBase * SGIProxyItemBase::realItem(bool getInstance)
{
    if (!_realItem.valid() && getInstance)
        _realItem = getRealInstance();
    return _realItem;
}
const SGIItemBase * SGIProxyItemBase::realItem(bool getInstance) const
{
    if (!_realItem.valid() && getInstance)
        const_cast<SGIProxyItemBase*>(this)->_realItem = const_cast<SGIProxyItemBase*>(this)->getRealInstance();
    return _realItem;
}


Image::Image(ImageFormat format)
    : _format(format), _origin(OriginDefault), _data(NULL), _length(0)
    , _width(0), _height(0), _depth(0), _pitch { 0, 0, 0, 0 }, _planeOffset{0, 0, 0, 0}
    , _originalImage(NULL), _originalImageQt(NULL)
{
}

Image::Image(ImageFormat format, Origin origin, void * data, size_t length,
        unsigned width, unsigned height, unsigned depth, unsigned bytesPerLine,
        const osg::Referenced * originalImage)
    : _format(format), _origin(origin), _data(data), _length(length)
    , _width(width), _height(height), _depth(depth), _pitch { bytesPerLine, 0, 0, 0 }, _planeOffset{0, 0, 0, 0}
    , _originalImage(originalImage), _originalImageQt(NULL)
{

}
Image::Image(ImageFormat format, Origin origin, void * data, size_t length,
    unsigned width, unsigned height, unsigned depth, unsigned bytesPerLine,
    QImage * originalImage)
    : _format(format), _origin(origin), _data(data), _length(length)
    , _width(width), _height(height), _depth(depth), _pitch { bytesPerLine, 0, 0, 0 }, _planeOffset{0, 0, 0, 0}
    , _originalImage(NULL), _originalImageQt((originalImage)?new QImage(*originalImage):NULL)
{

}

namespace {
    Image::ImageFormat imageFormatFromQImage(QImage::Format format)
    {
        Image::ImageFormat imageFormat;
        switch (format)
        {
        case QImage::Format_Invalid:imageFormat = sgi::Image::ImageFormatInvalid; break;
        case QImage::Format_Mono: imageFormat = sgi::Image::ImageFormatMono; break;
        case QImage::Format_MonoLSB: imageFormat = sgi::Image::ImageFormatMonoLSB; break;
        case QImage::Format_Indexed8: imageFormat = sgi::Image::ImageFormatIndexed8; break;
        case QImage::Format_RGB32: imageFormat = sgi::Image::ImageFormatRGB32; break;
        case QImage::Format_RGBA8888:
        case QImage::Format_RGBA8888_Premultiplied:
        case QImage::Format_ARGB32_Premultiplied:
        case QImage::Format_ARGB32: imageFormat = sgi::Image::ImageFormatARGB32; break;
        case QImage::Format_RGB888: imageFormat = sgi::Image::ImageFormatRGB24; break;
        case QImage::Format_RGB16:
        case QImage::Format_ARGB8565_Premultiplied:
        case QImage::Format_RGB666:
        case QImage::Format_ARGB6666_Premultiplied:
        case QImage::Format_RGB555:
        case QImage::Format_ARGB8555_Premultiplied:
        case QImage::Format_RGB444:
        case QImage::Format_ARGB4444_Premultiplied:
        case QImage::Format_RGBX8888:
        case QImage::Format_BGR30:
        case QImage::Format_A2BGR30_Premultiplied:
        case QImage::Format_RGB30:
        case QImage::Format_A2RGB30_Premultiplied:
        default:
            imageFormat = sgi::Image::ImageFormatRaw;
            break;
        }
        return imageFormat;
    }
}

Image::Image(QImage * originalImage)
    : _format(imageFormatFromQImage(originalImage->format()))
    , _origin(OriginTopLeft), _data(NULL), _length(0)
    , _width(originalImage->width()), _height(originalImage->height()), _depth(1), _pitch { (unsigned)originalImage->bytesPerLine(), 0, 0, 0 }
    , _planeOffset{0, 0, 0, 0}
    , _originalImage(NULL), _originalImageQt((originalImage) ? new QImage(*originalImage) : NULL)
{
    _data = _originalImageQt->bits();
    _length = _originalImageQt->byteCount();
}

Image::Image(const Image & rhs)
    : _format(rhs._format), _origin(rhs._origin), _data(rhs._data), _length(rhs._length)
    , _width(rhs._width), _height(rhs._height), _depth(rhs._depth)
    , _pitch { rhs._pitch[0], rhs._pitch[1], rhs._pitch[2], rhs._pitch[3] }
    , _planeOffset { rhs._planeOffset[0], rhs._planeOffset[1], rhs._planeOffset[2], rhs._planeOffset[3] }
    , _originalImage(rhs._originalImage), _originalImageQt(rhs._originalImageQt)
{

}

Image::~Image()
{
    if (_originalImageQt)
        delete _originalImageQt;
}

Image & Image::operator=(const Image & rhs)
{
    _format = rhs._format;
    _origin = rhs._origin;
    _data = rhs._data;
    _length = rhs._length;
    _width = rhs._width;
    _height = rhs._height;
    _depth = rhs._depth;
    _pitch[0] = rhs._pitch[0];
    _pitch[1] = rhs._pitch[1];
    _pitch[2] = rhs._pitch[2];
    _pitch[3] = rhs._pitch[3];
    _planeOffset[0] = rhs._planeOffset[0];
    _planeOffset[1] = rhs._planeOffset[1];
    _planeOffset[2] = rhs._planeOffset[2];
    _planeOffset[3] = rhs._planeOffset[3];
    _originalImage = rhs._originalImage;
    _originalImageQt = rhs._originalImageQt;
    return *this;
}


bool Image::allocate(unsigned width, unsigned height, ImageFormat format)
{
    bool ret = false;
    _length = 0;
    switch (format)
    {
    default:
    case ImageFormatInvalid:
        Q_ASSERT_X(false, __FUNCTION__, "invalid frame format");
        break;
    case ImageFormatAutomatic:
        Q_ASSERT_X(false, __FUNCTION__, "invalid frame format, automatic");
        break;
    case ImageFormatRaw:
        Q_ASSERT_X(false, __FUNCTION__, "invalid frame format, raw");
        break;
    case ImageFormatRGB24:
    case ImageFormatBGR24:
        {
            _length = width * height * 3;
            _pitch[0] = width * 3;
            _pitch[1] = _pitch[2] = _pitch[3] = 0;
            _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
        }
        break;
    case ImageFormatRGB32:
    case ImageFormatARGB32:
    case ImageFormatBGR32:
    case ImageFormatABGR32:
    case ImageFormatFloat:
        {
            _length = width * height * 4;
            _pitch[0] = width * 4;
            _pitch[1] = _pitch[2] = _pitch[3] = 0;
            _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
        }
        break;
    case ImageFormatYUV444:
        {
            _length = width * height * 3;
            _pitch[0] = _pitch[1] = _pitch[2] = width * 3;
            _pitch[3] = 0;
            _planeOffset[0] = 0;
            _planeOffset[1] = width * height * 3;
            _planeOffset[2] = _planeOffset[1] << 1;
            _planeOffset[3] = 0;
        }
        break;
    case ImageFormatYUV422:
        {
            _length = width * height * 2;
            _pitch[0] = width;
            _pitch[1] = _pitch[2] = width / 2;
            _pitch[3] = 0;
            _planeOffset[0] = 0;
            _planeOffset[1] = width * height;
            _planeOffset[2] = _planeOffset[1] + (_planeOffset[1] >> 1);
            _planeOffset[3] = 0;
        }
        break;
    case ImageFormatYUV420:
        {
            _length = width * height + (width / 2 * height/2);
            _pitch[0] = width;
            _pitch[1] = _pitch[2] = width / 2;
            _pitch[3] = 0;
            _planeOffset[0] = 0;
            _planeOffset[1] = width * height;
            _planeOffset[2] = _planeOffset[1] + (_planeOffset[1] >> 2);
            _planeOffset[3] = 0;
        }
        break;

    case ImageFormatYUYV:
    case ImageFormatUYVY:
        {
            _length = width * height * 2;
            _pitch[0] = width;
            _pitch[1] = _pitch[2] = _pitch[3] = 0;
            _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
        }
        break;
    case ImageFormatGray:
    case ImageFormatRed:
    case ImageFormatGreen:
    case ImageFormatBlue:
    case ImageFormatAlpha:
        {
            // only one channel with 8-bit color data
            _length = width * height;
            _pitch[0] = width;
            _pitch[1] = _pitch[2] = _pitch[3] = 0;
            _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
        }
        break;
    }
    if(_data)
    {
        free(_data);
        _data = NULL;
    }
    if(_length)
    {
        _data = malloc(_length);
        _width = width;
        _height = height;
        _format = format;
        ret = _data != NULL;
    }
    return ret;
}

std::string Image::imageFormatToString(ImageFormat format)
{
    std::string ret;
    switch(format)
    {
    case ImageFormatInvalid: ret = "Invalid"; break;
    case ImageFormatRGB24: ret = "RGB24"; break;
    case ImageFormatRGB32: ret = "RGB32"; break;
    case ImageFormatARGB32: ret = "ARGB32"; break;
    case ImageFormatMono: ret = "Mono"; break;
    case ImageFormatMonoLSB: ret = "MonoLSB"; break;
    case ImageFormatIndexed8: ret = "Indexed8"; break;
    case ImageFormatFloat: ret = "Float"; break;
    case ImageFormatBGR24: ret = "BGR24"; break;
    case ImageFormatBGR32: ret = "BGR32"; break;
    case ImageFormatABGR32: ret = "ABGR32"; break;
    case ImageFormatYUV420: ret = "YUV420"; break;
    case ImageFormatYUV422: ret = "YUV422"; break;
    case ImageFormatYUV444: ret = "YUV444"; break;
    case ImageFormatYUYV: ret = "YUYV"; break;
    case ImageFormatUYVY: ret = "UYVY"; break;
    case ImageFormatGray: ret = "Gray"; break;
    case ImageFormatRed: ret = "Red"; break;
    case ImageFormatGreen: ret = "Green"; break;
    case ImageFormatBlue: ret = "Blue"; break;
    case ImageFormatAlpha: ret = "Alpha"; break;
    case ImageFormatDepth: ret = "Depth"; break;
    case ImageFormatLuminance: ret = "Luminance"; break;
    case ImageFormatLuminanceAlpha: ret = "LuminanceAlpha"; break;
    case ImageFormatDXT1: ret = "DXT1"; break;
    case ImageFormatDXT1Alpha: ret = "DXT1a"; break;
    case ImageFormatDXT3: ret = "DXT3"; break;
    case ImageFormatDXT5: ret = "DXT5"; break;
    case ImageFormatRaw: ret = "raw"; break;
    default:
        {
            std::stringstream ss;
            ss << "Unknown(" << (int)format << ')';
            ret = ss.str();
        }
        break;
    }
    return ret;
}

std::string Image::originToString(Origin o)
{
    std::string ret;
    switch(o)
    {
    case OriginBottomLeft: ret = "bottom-left"; break;
    case OriginTopLeft: ret = "top-left"; break;
    default:
        {
            std::stringstream ss;
            ss << "Unknown(" << (int)o << ')';
            ret = ss.str();
        }
        break;
    }
    return ret;
}

}
