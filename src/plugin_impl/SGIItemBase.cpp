// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#include <sgi/plugins/SGIItemBase.h>
#include <sstream>

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




Image::Image(ImageFormat format, Origin origin, void * data, size_t length,
        unsigned width, unsigned height, unsigned depth, unsigned bytesPerLine,
        osg::Referenced * originalImage)
    : _format(format), _origin(origin), _data(data), _length(length)
    , _width(width), _height(height), _depth(depth), _bytesPerLine(bytesPerLine)
    , _originalImage(originalImage), _originalImageQt(NULL)
{

}
Image::Image(ImageFormat format, Origin origin, void * data, size_t length,
    unsigned width, unsigned height, unsigned depth, unsigned bytesPerLine,
    QImage * originalImage)
    : _format(format), _origin(origin), _data(data), _length(length)
    , _width(width), _height(height), _depth(depth), _bytesPerLine(bytesPerLine)
    , _originalImage(NULL), _originalImageQt(originalImage)
{

}
Image::Image(const Image & rhs)
    : _format(rhs._format), _origin(rhs._origin), _data(rhs._data), _length(rhs._length)
    , _width(rhs._width), _height(rhs._height), _depth(rhs._depth), _bytesPerLine(rhs._bytesPerLine)
    , _originalImage(rhs._originalImage), _originalImageQt(rhs._originalImageQt)
{

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
    _bytesPerLine = rhs._bytesPerLine;
    _originalImage = rhs._originalImage;
    _originalImageQt = rhs._originalImageQt;
    return *this;
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
    case ImageFormatARGB32_Premultiplied: ret = "ARGB32_Premultiplied"; break;
    case ImageFormatMono: ret = "Mono"; break;
    case ImageFormatMonoLSB: ret = "MonoLSB"; break;
    case ImageFormatIndexed8: ret = "Indexed8"; break;
    case ImageFormatFloat: ret = "Float"; break;
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
