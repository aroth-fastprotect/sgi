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
    , _allocated(false)
{
}

Image::Image(ImageFormat format, Origin origin, void * data, size_t length,
        unsigned width, unsigned height, unsigned depth, unsigned bytesPerLine,
        const osg::Referenced * originalImage)
    : _format(format), _origin(origin), _data(data), _length(length)
    , _width(width), _height(height), _depth(depth), _pitch { bytesPerLine, 0, 0, 0 }, _planeOffset{0, 0, 0, 0}
    , _originalImage(originalImage), _originalImageQt(NULL)
    , _allocated(false)
{

}
Image::Image(ImageFormat format, Origin origin, void * data, size_t length,
    unsigned width, unsigned height, unsigned depth, unsigned bytesPerLine,
    QImage * originalImage)
    : _format(format), _origin(origin), _data(data), _length(length)
    , _width(width), _height(height), _depth(depth), _pitch { bytesPerLine, 0, 0, 0 }, _planeOffset{0, 0, 0, 0}
    , _originalImage(NULL), _originalImageQt((originalImage)?new QImage(*originalImage):NULL)
    , _allocated(false)
{

}

namespace {
    Image::ImageFormat imageFormatFromQImage(QImage::Format format)
    {
        Image::ImageFormat imageFormat;
        switch (format)
        {
        case QImage::Format_Invalid:imageFormat = Image::ImageFormatInvalid; break;
        case QImage::Format_Mono: imageFormat = Image::ImageFormatMono; break;
        case QImage::Format_MonoLSB: imageFormat = Image::ImageFormatMonoLSB; break;
        case QImage::Format_Indexed8: imageFormat = Image::ImageFormatIndexed8; break;
        case QImage::Format_RGB32: imageFormat = Image::ImageFormatRGB32; break;
        case QImage::Format_RGBA8888:
        case QImage::Format_RGBA8888_Premultiplied:
        case QImage::Format_ARGB32_Premultiplied:
        case QImage::Format_ARGB32: imageFormat = Image::ImageFormatARGB32; break;
        case QImage::Format_RGB888: imageFormat = Image::ImageFormatRGB24; break;
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
            imageFormat = Image::ImageFormatRaw;
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
    , _allocated(false)
{
    _data = _originalImageQt->bits();
    _length = _originalImageQt->byteCount();
}

Image::Image(ImageFormat format, void * data, size_t length, bool copyData)
    : _format(format), _origin(OriginDefault), _data(copyData ? malloc(length) : data), _length(length)
    , _width(0), _height(0), _depth(0), _pitch{ 0, 0, 0, 0 }, _planeOffset{ 0, 0, 0, 0 }
    , _originalImage(NULL), _originalImageQt(NULL)
    , _allocated(copyData)
{
    if (copyData)
        memcpy(_data, data, length);
}

Image::Image(const Image & rhs)
    : _format(rhs._format), _origin(rhs._origin), _data(rhs._data), _length(rhs._length)
    , _width(rhs._width), _height(rhs._height), _depth(rhs._depth)
    , _pitch { rhs._pitch[0], rhs._pitch[1], rhs._pitch[2], rhs._pitch[3] }
    , _planeOffset { rhs._planeOffset[0], rhs._planeOffset[1], rhs._planeOffset[2], rhs._planeOffset[3] }
    , _originalImage(rhs._originalImage), _originalImageQt(rhs._originalImageQt)
    , _allocated(false)
{

}

Image::~Image()
{
    free();
}

Image & Image::operator=(const Image & rhs)
{
    free();
    _format = rhs._format;
    _origin = rhs._origin;
    _data = rhs._data;
    _allocated = false;
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

void Image::free()
{
    if (_originalImageQt)
    {
        delete _originalImageQt;
        _originalImageQt = NULL;
    }
    if(_allocated && _data)
    {
        ::free(_data);
        _data = NULL;
    }
}


bool Image::allocate(unsigned width, unsigned height, ImageFormat format)
{
    bool ret = false;
    free();
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
    if(_length)
    {
        _data = malloc(_length);
        _width = width;
        _height = height;
        _format = format;
        ret = _data != NULL;
        _allocated = ret;
    }
    return ret;
}

bool Image::reinterpretFormat(ImageFormat targetFormat)
{
    bool ret = false;
    switch(_format)
    {
    case Image::ImageFormatRed:
    case Image::ImageFormatGreen:
    case Image::ImageFormatBlue:
    case Image::ImageFormatAlpha:
    case Image::ImageFormatGray:
        {
            // reinterpret a single channel image as a full color image
            switch(targetFormat)
            {
            case Image::ImageFormatYUV444:
                ret = (_height % 3) == 0;
                if(ret)
                {
                    unsigned planeHeight = _height / 3;
                    unsigned planeLength = planeHeight * _width;
                    // split single plane into three separate ones
                    _planeOffset[0] = 0;
                    _planeOffset[1] = planeLength;
                    _planeOffset[2] = planeLength + planeLength;
                    _planeOffset[3] = 0;
                    _pitch[0] = _width;
                    _pitch[1] = _width;
                    _pitch[2] = _width;
                    _pitch[3] = 0;
                    _height = planeHeight;
                }
                break;
            case Image::ImageFormatYUV422:
                ret = (_height % 2) == 0;
                if(ret)
                {
                    unsigned luma_planeHeight = _height / 2;
                    unsigned chroma_planeHeight = luma_planeHeight / 2;
                    unsigned luma_planeLength = luma_planeHeight * _width;
                    unsigned chroma_planeLength = chroma_planeHeight * _width;
                    // split single plane into three separate ones
                    _planeOffset[0] = 0;
                    _planeOffset[1] = luma_planeLength;
                    _planeOffset[2] = luma_planeLength + chroma_planeLength;
                    _planeOffset[3] = 0;
                    _pitch[0] = _width;
                    _pitch[1] = _width;
                    _pitch[2] = _width;
                    _pitch[3] = 0;
                    _height = luma_planeHeight;
                }
                break;
            case Image::ImageFormatYUV420:
                ret = ((_height+_height) % 3) == 0;
                if(ret)
                {
                    unsigned luma_planeHeight = (_height + _height)/ 3;
                    unsigned chroma_planeHeight = luma_planeHeight / 2;
                    unsigned luma_planeLength = luma_planeHeight * _width;
                    unsigned chroma_planeLength = chroma_planeHeight * (_width/2);
                    // split single plane into three separate ones
                    _planeOffset[0] = 0;
                    _planeOffset[1] = luma_planeLength;
                    _planeOffset[2] = luma_planeLength + chroma_planeLength;
                    _planeOffset[3] = 0;
                    _pitch[0] = _width;
                    _pitch[1] = _width / 2;
                    _pitch[2] = _width / 2;
                    _pitch[3] = 0;
                    _height = luma_planeHeight;
                }
                break;
            case Image::ImageFormatYUYV:
            case Image::ImageFormatUYVY:
                {
                    // it's a YUV422 color format all in one plane
                    ret = (_height % 2) == 0;
                    if(ret)
                    {
                        unsigned luma_planeHeight = _height / 2;
                        // split single plane into three separate ones
                        _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
                        _pitch[0] = _width;
                        _pitch[1] = _pitch[2] = _pitch[3] = 0;
                        _height = luma_planeHeight;
                    }
                }
                break;
            }
        }
        break;
    case ImageFormatBGR32:
    case ImageFormatRGB32:
    case ImageFormatARGB32:
    case ImageFormatABGR32:
        // reinterpret a 32-bit image and reinterpret the colors (helpful for wrong color
        // display)
        switch(targetFormat)
        {
        case ImageFormatBGR32:
        case ImageFormatRGB32:
        case ImageFormatARGB32:
        case ImageFormatABGR32:
            // just accept this, no special handling required
            ret = true;
        }
        break;
    case ImageFormatBGR24:
    case ImageFormatRGB24:
    case ImageFormatYUV444:
        // reinterpret a 24-bit image and reinterpret the colors (helpful for wrong color
        // display)
        switch(targetFormat)
        {
        case ImageFormatBGR24:
        case ImageFormatRGB24:
        case ImageFormatYUV444:
            // just accept this, no special handling required
            ret = true;
        }
        break;
    }
    if(ret)
    {
        // we got the desired format, so take the targetFormat as new format
        _format = targetFormat;
    }
    return ret;
}

bool Image::reinterpret(const ImageSize & size)
{
    return reinterpret(size.format, size.width, size.height, size.depth);
}

bool Image::reinterpret(ImageFormat format, unsigned width, unsigned height, unsigned depth)
{
    _width = width;
    _height = height;
    _depth = depth;
    _format = format;
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
            _pitch[0] = width * 4;
            _pitch[1] = _pitch[2] = _pitch[3] = 0;
            _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
        }
        break;
    case ImageFormatYUV444:
        {
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
            _pitch[0] = width;
            _pitch[1] = _pitch[2] = _pitch[3] = 0;
            _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
        }
        break;
    }
    return true;
}

bool Image::guessImageSizes(ImageSizeList & possibleSizes) const
{
    bool ret = false;
    possibleSizes.clear();
    quint64 totalNumberOfPixels = 0;
    switch(_format)
    {
    case Image::ImageFormatARGB32:
    case Image::ImageFormatRGB32:
    case Image::ImageFormatABGR32:
    case Image::ImageFormatBGR32:
    case Image::ImageFormatFloat:
        ret = (_length % 4 == 0);
        if(ret)
            totalNumberOfPixels = _length / 4;
        break;
    case Image::ImageFormatRGB24:
    case Image::ImageFormatBGR24:
    case Image::ImageFormatYUV444:
        ret = (_length % 3 == 0);
        if(ret)
            totalNumberOfPixels = _length / 3;
        break;
    case Image::ImageFormatYUV422:
    case Image::ImageFormatYUYV:
    case Image::ImageFormatUYVY:
        ret = (_length % 2 == 0);
        if(ret)
            totalNumberOfPixels = _length / 2;
        break;
    case Image::ImageFormatYUV420:
        ret = ((_length*2) % 3 == 0);
        if(ret)
            totalNumberOfPixels = (_length*2) / 3;
    }
    if(ret)
    {
        double d = sqrt(totalNumberOfPixels);
        if(fmod(d, 1.0) == 0)
        {
            // got a square image
            int width = std::floor(d);
            int height = width;
            possibleSizes.push_back(ImageSize(_format, width, height));
        }
        else
        {
            const int size_step_size = 8;
            const int min_height = 16;
            const int min_width = 16;
            int max_width = totalNumberOfPixels / min_height;
            int start_width = (int)std::floor(d)/size_step_size * size_step_size;
            // adjust to next matching 8-pixel width
            start_width = (start_width / size_step_size) * size_step_size;

            for(int width = start_width; width <= max_width; width += size_step_size)
            {
                int height_4_3 = (width * 3) / 4;
                int height_16_9 = (width * 9) / 16;
                int total_4_3 = width * height_4_3;
                int total_16_9 = width * height_16_9;
                if(total_4_3 == totalNumberOfPixels)
                {
                    possibleSizes.push_back(ImageSize(_format, width, height_4_3));
                }
                if(total_16_9 == totalNumberOfPixels)
                {
                    possibleSizes.push_back(ImageSize(_format, width, height_16_9));
                }
            }
            for(int width = start_width; width >= min_width; width -= size_step_size)
            {
                int height_4_3 = (width * 3) / 4;
                int height_16_9 = (width * 9) / 16;
                int total_4_3 = width * height_4_3;
                int total_16_9 = width * height_16_9;
                if(total_4_3 == totalNumberOfPixels)
                {
                    possibleSizes.push_back(ImageSize(_format, width, height_4_3));
                }
                if(total_16_9 == totalNumberOfPixels)
                {
                    possibleSizes.push_back(ImageSize(_format, width, height_16_9));
                }
            }

        }
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
