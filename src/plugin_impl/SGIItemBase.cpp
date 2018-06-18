// kate: syntax C++;
// SGI - Copyright (C) 2012-2018 FAST Protect, Andreas Roth

#include <sgi/plugins/SGIItemBase.h>
#include <sgi/plugins/SGIProxyItem.h>
#include <sgi/plugins/Matrix>
#include <sgi/plugins/Quat>
#include <sstream>
#include <cmath>

#include <QImage>

#define SGI_DEBUG_ITEM_ALLOCATION

#ifdef SGI_DEBUG_ITEM_ALLOCATION
#include <QDebug>
#define SGI_ITEM_LOG(msg)   qDebug() << msg
#else
#define SGI_ITEM_LOG(msg)   (void(0))
#endif

namespace sgi {
// some method implementations from SGIItemBase which are only
// used within the SGI base and not in any plugin or caller.

namespace math {
#if defined(_MSC_VER)
    inline bool isNaN(double v) { return _isnan(v)!=0; }
#elif defined(__ANDROID__)
    inline bool isNaN(float v) { return isnan(v); }
    inline bool isNaN(double v) { return isnan(v); }
#else
    inline bool isNaN(float v) { return std::isnan(v); }
    inline bool isNaN(double v) { return std::isnan(v); }
#endif
} // namespace math

namespace {
    static std::atomic_uint s_ItemCount(0);
    static std::atomic_uint s_ItemHolderCount(0);
}

unsigned SGIItemHolder::getTotalItemCount()
{
    return s_ItemHolderCount;
}

SGIItemHolder::~SGIItemHolder()
{
    --s_ItemHolderCount;
    SGI_ITEM_LOG(__FUNCTION__ << (void*)this);
}

SGIItemHolder::SGIItemHolder()
{
    ++s_ItemHolderCount;
    SGI_ITEM_LOG(__FUNCTION__ << (void*)this);
}

SGIItemHolder::SGIItemHolder(const SGIItemHolder & /*rhs*/)
{
    ++s_ItemHolderCount;
    SGI_ITEM_LOG(__FUNCTION__ << (void*)this);
}

unsigned SGIItemBase::getTotalItemCount()
{
    return s_ItemCount;
}

SGIItemBase::SGIItemBase(SGIItemHolder * holder, SGIItemType type, unsigned flags, unsigned score, details::Referenced * userData)
    : details::Object(), _holder(holder), _type(type), _flags(flags), _score(score)
    , _pluginInfo(nullptr), _type_info(nullptr), _next(nullptr), _prev()
    , _number(0), _userData(userData)
{
    SGI_ITEM_LOG(__FUNCTION__ << (void*)this);
    ++s_ItemCount;
}

SGIItemBase::SGIItemBase(const SGIItemBase & rhs)
    : details::Object(rhs), _holder(rhs._holder), _type(rhs._type), _flags(rhs._flags), _score(rhs._score)
    , _pluginInfo(rhs._pluginInfo), _type_info(rhs._type_info), _next(rhs._next), _prev(rhs._prev)
    , _number(rhs._number), _userData(rhs._userData)
{
    SGI_ITEM_LOG(__FUNCTION__ << (void*)this << (void*)&rhs);
    ++s_ItemCount;
}

SGIItemBase::~SGIItemBase()
{
    _holder = nullptr;
    _pluginInfo = nullptr;
    _next = nullptr;
    _prev = nullptr;
    _userData = nullptr;
    --s_ItemCount;
    SGI_ITEM_LOG(__FUNCTION__ << (void*)this);
}

SGIItemBase & SGIItemBase::operator = (const SGIItemBase & rhs)
{
    SGI_ITEM_LOG(__FUNCTION__ << (void*)this << (void*)&rhs);
    _holder = rhs._holder;
    _type = rhs._type;
    _flags = rhs._flags;
    _score = rhs._score;
    _pluginInfo = rhs._pluginInfo;
    _type_info = rhs._type_info;
    _next = rhs._next;
    _prev = rhs._prev;
    _number = rhs._number;
    _userData = rhs._userData;
    return *this;
}

bool SGIItemBase::operator == (const SGIItemBase & rhs) const
{
    return compare(rhs) == 0;
}

bool SGIItemBase::operator != (const SGIItemBase & rhs) const
{
    return compare(rhs) != 0;
}


int SGIItemBase::compare(const SGIItemBase & rhs) const
{
    if(rhs._type == _type)
    {
        if(rhs._holder == _holder)
            return 0;
        else if(!_holder)
            return -1;
        else if(!rhs._holder)
            return 1;
        else
            return _holder->compare(*rhs._holder);
    }
    else if(rhs._type < _type)
        return -1;
    else
        return 1;
}

bool SGIItemBase::valid() const { return _type != SGIItemTypeInvalid; }
SGIItemType SGIItemBase::type() const { return _type; }
void SGIItemBase::setType(SGIItemType type) { _type = type; }
unsigned SGIItemBase::score() const { return _score; }
unsigned SGIItemBase::incrementScore() { return ++_score; }
unsigned SGIItemBase::decrementScore() { return --_score; }


/// @brief override the plugin info in the current item in list
/// @param pluginInfo pointer to plugin info
void SGIItemBase::setPluginInfo(const ISGIPluginInfo * pluginInfo)
{
    _pluginInfo = pluginInfo;
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
            nextItemToInsert->_prev = nullptr;
        itemToInsert->_next = nullptr;

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
                insertionPoint->insertBefore(itemToInsert.get());
                if(insertionPoint == front)
                    front = itemToInsert;
            }
            else
                insertionPoint->insertAfter(itemToInsert.get());
        }
        else if(itemToInsert->_score > insertionPoint->_score)
        {
            insertionPoint->insertBefore(itemToInsert.get());
            if(insertionPoint == front)
                front = itemToInsert;
        }
        else
            insertionPoint->insertAfter(itemToInsert.get());
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

SGIItemBase * SGIItemBase::cloneImpl(SGIItemType newType)
{
    SGIItemBasePtr ret;
    SGIItemBasePtr previous_cloned;
    SGIItemBasePtr current = this;
    SGIItemBasePtr safe_this = this;
    while(current.valid())
    {
        SGIItemBasePtr clonedItem = (SGIItemBase*)current->clone();
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
    // release any temp references
    previous_cloned = nullptr;
    current = nullptr;
    return ret.release();
}

SGIItemBase * SGIItemBase::cloneImpl(SGIItemType newType, unsigned number)
{
    SGIItemBasePtr ret;
    SGIItemBasePtr previous_cloned;
    SGIItemBasePtr current = this;
    SGIItemBasePtr safe_this = this;
    while(current.valid())
    {
        SGIItemBasePtr clonedItem = (SGIItemBase*)current->clone();
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
    // release any temp references
    previous_cloned = nullptr;
    current = nullptr;
    return ret.release();
}

SGIItemBase * SGIItemBase::cloneImpl(SGIItemType newType, details::Referenced * userData)
{
    SGIItemBasePtr ret;
    SGIItemBasePtr previous_cloned;
    SGIItemBasePtr current = this;
    while(current.valid())
    {
        SGIItemBasePtr clonedItem = (SGIItemBase*)current->clone();
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
    // release any temp references
    previous_cloned = nullptr;
    current = nullptr;
    return ret.release();
}

SGIItemBase * SGIItemBase::cloneImpl(SGIItemType newType, unsigned number, details::Referenced * userData)
{
    SGIItemBasePtr ret;
    SGIItemBasePtr previous_cloned;
    SGIItemBasePtr current = this;
    while(current.valid())
    {
        SGIItemBasePtr clonedItem = (SGIItemBase*)current->clone();
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
    // release any temp references
    previous_cloned = nullptr;
    current = nullptr;
    return ret.release();
}

SGIProxyItemBase::SGIProxyItemBase(SGIPluginHostInterface * hostInterface, SGIItemBase * realItem)
    : _realItem(realItem), _hostInterface(hostInterface)
{
}

SGIProxyItemBase::SGIProxyItemBase(SGIPluginHostInterface * hostInterface, 
                                    const std::string & name, 
                                    const std::string & typeName, 
                                    const std::string & displayName, 
                                    SGIItemBase * realItem)
    : _name(name), _displayName(displayName), _typeName(typeName)
    , _realItem(realItem) 
    , _hostInterface(hostInterface)
{
}

SGIProxyItemBase::SGIProxyItemBase(const SGIProxyItemBase & rhs)
    : _name(rhs._name), _displayName(rhs._displayName)
    , _typeName(rhs._typeName), _realItem(rhs._realItem) 
    , _hostInterface(rhs._hostInterface)
{
}

SGIItemBase * SGIProxyItemBase::realItem(bool getInstance)
{
    if (!_realItem.valid() && getInstance)
        _realItem = getRealInstance();
    return _realItem.get();
}
const SGIItemBase * SGIProxyItemBase::realItem(bool getInstance) const
{
    if (!_realItem.valid() && getInstance)
        const_cast<SGIProxyItemBase*>(this)->_realItem = const_cast<SGIProxyItemBase*>(this)->getRealInstance();
    return _realItem.get();
}


Quat::value_type Quat::length() const
{
    return sqrt( _v[0]*_v[0] + _v[1]*_v[1] + _v[2]*_v[2] + _v[3]*_v[3]);
}

bool Quat::isNaN() const
{
    return math::isNaN(_v[0]) || math::isNaN(_v[1]) || math::isNaN(_v[2]) || math::isNaN(_v[3]);
}


#define SET_ROW(row, v1, v2, v3, v4 )    \
    _mat[(row)][0] = (v1); \
    _mat[(row)][1] = (v2); \
    _mat[(row)][2] = (v3); \
    _mat[(row)][3] = (v4);

#define INNER_PRODUCT(a,b,r,c) \
     ((a)._mat[r][0] * (b)._mat[0][c]) \
    +((a)._mat[r][1] * (b)._mat[1][c]) \
    +((a)._mat[r][2] * (b)._mat[2][c]) \
    +((a)._mat[r][3] * (b)._mat[3][c])


Matrix::Matrix( value_type a00, value_type a01, value_type a02, value_type a03,
                  value_type a10, value_type a11, value_type a12, value_type a13,
                  value_type a20, value_type a21, value_type a22, value_type a23,
                  value_type a30, value_type a31, value_type a32, value_type a33)
{
    SET_ROW(0, a00, a01, a02, a03 )
    SET_ROW(1, a10, a11, a12, a13 )
    SET_ROW(2, a20, a21, a22, a23 )
    SET_ROW(3, a30, a31, a32, a33 )
}

void Matrix::set( value_type a00, value_type a01, value_type a02, value_type a03,
                   value_type a10, value_type a11, value_type a12, value_type a13,
                   value_type a20, value_type a21, value_type a22, value_type a23,
                   value_type a30, value_type a31, value_type a32, value_type a33)
{
    SET_ROW(0, a00, a01, a02, a03 )
    SET_ROW(1, a10, a11, a12, a13 )
    SET_ROW(2, a20, a21, a22, a23 )
    SET_ROW(3, a30, a31, a32, a33 )
}

void Matrix::makeIdentity()
{
    SET_ROW(0,    1, 0, 0, 0 )
    SET_ROW(1,    0, 1, 0, 0 )
    SET_ROW(2,    0, 0, 1, 0 )
    SET_ROW(3,    0, 0, 0, 1 )
}

void Matrix::makeRotate( const Quat& quat )
{
    makeIdentity();

    setRotate(quat);
}

bool Matrix::isNaN() const
{
    return math::isNaN(_mat[0][0]) || math::isNaN(_mat[0][1]) || math::isNaN(_mat[0][2]) || math::isNaN(_mat[0][3]) ||
            math::isNaN(_mat[1][0]) || math::isNaN(_mat[1][1]) || math::isNaN(_mat[1][2]) || math::isNaN(_mat[1][3]) ||
            math::isNaN(_mat[2][0]) || math::isNaN(_mat[2][1]) || math::isNaN(_mat[2][2]) || math::isNaN(_mat[2][3]) ||
            math::isNaN(_mat[3][0]) || math::isNaN(_mat[3][1]) || math::isNaN(_mat[3][2]) || math::isNaN(_mat[3][3]);

}


#define QX  q._v[0]
#define QY  q._v[1]
#define QZ  q._v[2]
#define QW  q._v[3]

void Matrix::setRotate(const Quat& q)
{
    double length2 = q.length2();
    if (std::fabs(length2) <= std::numeric_limits<double>::min())
    {
        _mat[0][0] = 0.0; _mat[1][0] = 0.0; _mat[2][0] = 0.0;
        _mat[0][1] = 0.0; _mat[1][1] = 0.0; _mat[2][1] = 0.0;
        _mat[0][2] = 0.0; _mat[1][2] = 0.0; _mat[2][2] = 0.0;
    }
    else
    {
        double rlength2;
        // normalize quat if required.
        // We can avoid the expensive sqrt in this case since all 'coefficients' below are products of two q components.
        // That is a square of a square root, so it is possible to avoid that
        if (length2 != 1.0)
        {
            rlength2 = 2.0/length2;
        }
        else
        {
            rlength2 = 2.0;
        }

        // Source: Gamasutra, Rotating Objects Using Quaternions
        //
        //http://www.gamasutra.com/features/19980703/quaternions_01.htm

        double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

        // calculate coefficients
        x2 = rlength2*QX;
        y2 = rlength2*QY;
        z2 = rlength2*QZ;

        xx = QX * x2;
        xy = QX * y2;
        xz = QX * z2;

        yy = QY * y2;
        yz = QY * z2;
        zz = QZ * z2;

        wx = QW * x2;
        wy = QW * y2;
        wz = QW * z2;

        // Note.  Gamasutra gets the matrix assignments inverted, resulting
        // in left-handed rotations, which is contrary to OpenGL and OSG's
        // methodology.  The matrix assignment has been altered in the next
        // few lines of code to do the right thing.
        // Don Burns - Oct 13, 2001
        _mat[0][0] = 1.0 - (yy + zz);
        _mat[1][0] = xy - wz;
        _mat[2][0] = xz + wy;


        _mat[0][1] = xy + wz;
        _mat[1][1] = 1.0 - (xx + zz);
        _mat[2][1] = yz - wx;

        _mat[0][2] = xz - wy;
        _mat[1][2] = yz + wx;
        _mat[2][2] = 1.0 - (xx + yy);
    }

#if 0
    _mat[0][3] = 0.0;
    _mat[1][3] = 0.0;
    _mat[2][3] = 0.0;

    _mat[3][0] = 0.0;
    _mat[3][1] = 0.0;
    _mat[3][2] = 0.0;
    _mat[3][3] = 1.0;
#endif
}

} // namespace sgi
